#include <octomap/HashMap.h>
#include <octomap/OcTree.h>
#include <octomap/GlobalVariables_Octomap.h>
#include <octomap/GlobalVariables_Cache.h>

#define DEBUG3 true

namespace octomap{

void HashMap::KickToBuffer(ReaderWriterQueue<Item>* q, std::atomic_int& bufferSize){
// remove all KV at position clock
    auto it = table[clock].begin();
    while (it != table[clock].end()){
        // kick that entry
        Item item = Item(it->key, it->occupancy);
        q->enqueue(item);
#if DETAIL_COUNT
        insert_to_buffer++;
#endif
        bufferSize++;
        table[clock].erase(it);
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
        // kick that entry
        Item item = Item(key, it->occupancy);
        tree->updateNode(key, item.occupancy, lazy_eval);
        table[clock].erase(it);
    }
    clock = (clock + 1) % TABLE_SIZE;
}

void HashMap::Kick(uint32_t num, ReaderWriterQueue<Item>* q, std::atomic_int& bufferSize) {
    while (num > 0) {
        if (table[clock].size() == 0) {
            clock = (clock + 1) % TABLE_SIZE;
        }
        else {
            num -= table[clock].size();
            auto it = table[clock].begin();
            while (it != table[clock].end()){
                // kick that entry
                Item item = Item(it->key, it->occupancy);
                q->enqueue(item);
        #if DETAIL_COUNT
                insert_to_buffer++;
        #endif
                bufferSize++;
                table[clock].erase(it);
                if (it == table[clock].end()){
                    break;
                }
                else{
                    it++;
                }
            }
            clock = (clock + 1) % TABLE_SIZE;
        }
    }
}

float HashMap::get(const OcTreeKey &key) {
    // this function waits to be finished
    // search each bucket after another, stops if found 10 consecutive full buckets
    unsigned long hashValue = MortonHash(key);
    for (auto it = table[hashValue].begin(); it != table[hashValue].end(); it++) {
        if (it->key == key){
            return it->occupancy;
        }
    }
    // it is impossible to return a so large number as 100, note as not found
    return bigNumber;
}


void HashMap::put(const OcTreeKey &key, const bool &value, const uint32_t& hashValue) {
#if DETAIL_COUNT
    insert_to_hashmap++;
#endif
    // std::cout << table[hashValue].size() << std::endl;
    for (auto it = table[hashValue].begin(); it != table[hashValue].end(); it++) {
        if (it->key == key){
            // std::cout << "found" << std::endl;
            if (value == true) {
                it->occupancy += tree->getProbHitLog();
                if (it->occupancy > tree->getClampingThresMaxLog()) {
                    it->occupancy = tree->getClampingThresMaxLog();
                }
                return;
            }
            else {
                it->occupancy += tree->getProbMissLog();
                if (it->occupancy < tree->getClampingThresMinLog()) {
                    it->occupancy = tree->getClampingThresMinLog();
                }
                return;
            }
        }
    }
    // std::cout << "not found" << std::endl;

    // this node does not exist
    double accumulateOccupancy = 0.0;
    // std::cout << key.k[0] << " " << key.k[1] << " " << key.k[2] << std::endl;
    auto node = tree->search(key);
    // std::cout << "search finished" << std::endl;
    fetch_from_octree++;
    if (node == NULL) {
        accumulateOccupancy = 0.0;
    }
    else{
        accumulateOccupancy = node->getOccupancy();
    }
    // std::cout << "occupancy = " << accumulateOccupancy << std::endl;
    table[hashValue].push_back(HashNode(key, accumulateOccupancy));   
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
    for (int i = 9; i >= 0; i--) {
        sum *= 8;
        sum += k0.test(i) * 4 + k1.test(i) * 2 + k2.test(i);
    }
    uint32_t hashValue = sum % TABLE_SIZE;
    // uint32_t hashValue = hashFunc.run((const char *)(&(key.k[0])), sizeof(key_type) * 3) % TABLE_SIZE;
    return hashValue;
}

uint32_t HashMap::RoundRobin(uint32_t count){
    return (count % TABLE_SIZE);
}


void HashMap::cleanHashMap(ReaderWriterQueue<Item>* q, std::atomic_int& bufferSize) {
    currentPointCloud++;
    printf("Cleaning the HashMap\n");
    for (uint32_t i = 0; i < TABLE_SIZE; i++) {
        KickToBuffer(q, bufferSize);
    }
}

int HashMap::loadSize(){
    int sum = 0;
    for (int i = 0; i < TABLE_SIZE; i++) {
        sum += table[i].size();
    }
}

}