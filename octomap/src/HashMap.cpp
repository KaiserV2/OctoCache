#include <octomap/HashMap.h>
#include <octomap/OcTree.h>

#define DEBUG3 true

namespace octomap{

void HashMap::KickToBuffer(ReaderWriterQueue<Item>* q, std::atomic_int& bufferSize){
// remove all KV at position clock
    auto it = table[clock].begin();
    while (it != table[clock].end()){
        if ((it->myValue.currentPointCloud != currentPointCloud)) {
            OcTreeKey key = it->getKey();
            // first compress that result
            it->myValue.compress(tree->getProbMissLog(), tree->getProbHitLog());
            // kick that entry
            Item item = Item(key, it->myValue.accumulateOccupancy);
            q->enqueue(item);
#if DETAIL_COUNT
            insert_to_buffer++;
#endif
            bufferSize++;
            table[clock].erase(it);
        }
        if (it == table[clock].end()){
            break;
        }
        else{
            it++;
        }
    }
    clock = (clock + 1) % TABLE_SIZE;
}


void HashMap::KickToOctree() {
// remove all KV at position clock
    for (auto it = table[clock].begin(); it != table[clock].end(); it++){
        OcTreeKey key = it->getKey();
        if ((it->myValue.currentPointCloud == currentPointCloud)) {
            // don't kick that entry because the current stage is updating it
            continue;
        }
        else{
            // first compress that result
            it->myValue.compress(tree->getProbMissLog(), tree->getProbHitLog());
            // kick that entry
            Item item = Item(key, it->myValue.accumulateOccupancy);
            tree->updateNode(key, item.occupancy, lazy_eval);
            table[clock].erase(it);
        }
    }
    clock = (clock + 1) % TABLE_SIZE;
}




void HashMap::put(const OcTreeKey &key, const bool &value, const uint32_t& hashValue) {
#if DETAIL_COUNT
    insert_to_hashmap++;
#endif
    bool exist = false;

    for (auto it = table[hashValue].begin(); it != table[hashValue].end(); it++) {
        if (it->getKey() == key){
            exist = true;
            if (it->myValue.currentPointCloud == currentPointCloud) {
            // updating in the current point cloud
            it->myValue.currentOccupancy = it->myValue.currentOccupancy || value;
            }
            else{
                // comes in with a new point cloud
                // first merge information of the last round
                it->myValue.compress(tree->getProbMissLog(), tree->getProbHitLog());
                // then set up for new point cloud statistics
                it->myValue.currentPointCloud = currentPointCloud;
                it->myValue.currentOccupancy = value;
            }
            return;
        }
    }
    
    if (exist == false) {
        // this node does not exist
        double accumulateOccupancy;
        if (tree->search(key) == NULL) {
#if DETAIL_COUNT
                fetch_from_octree++;
#endif
            accumulateOccupancy = 0.0;
        }
        else{
            accumulateOccupancy = tree->search(key)->getOccupancy();
        }
        MyValue myValue = MyValue(accumulateOccupancy, value, currentPointCloud);
        table[hashValue].push_back(HashNode(key, myValue));
    }      
}



uint32_t HashMap::ScalarHash(const OcTreeKey &key){
    uint32_t keys[2] = {0, 0};
    memcpy((uint16_t*)keys, &key.k[0], 6);
    uint32_t hashValue = murmur3<2>::scalar((uint32_t *)(&(keys[0])), 1) % TABLE_SIZE;
    // uint32_t hashValue = hashFunc.run((const char *)(&(key.k[0])), sizeof(key_type) * 3) % TABLE_SIZE;
    return hashValue;
}



uint32_t HashMap::MortonHash(const OcTreeKey &key){
    uint64_t sum = 0;
    std::bitset<16> k0(key.k[0]);
    std::bitset<16> k1(key.k[1]);
    std::bitset<16> k2(key.k[2]);
    for (int i = 15; i >= 0; i--) {
        sum *= 8;
        sum += k0.test(i) * 4 + k1.test(i) * 2 + k2.test(i);
    }
    uint32_t hashValue = sum % TABLE_SIZE;
    // uint32_t hashValue = hashFunc.run((const char *)(&(key.k[0])), sizeof(key_type) * 3) % TABLE_SIZE;
    return hashValue;
}


void HashMap::cleanHashMap(ReaderWriterQueue<Item>* q, std::atomic_int& bufferSize) {
    currentPointCloud++;
    printf("Cleaning the HashMap\n");
    for (uint32_t i = 0; i < TABLE_SIZE; i++) {
        KickToBuffer(q, bufferSize);
    }
}

}