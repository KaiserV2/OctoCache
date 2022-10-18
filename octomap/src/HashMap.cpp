#include <octomap/HashMap.h>
#include <octomap/OcTree.h>

#define DEBUG3 true

namespace octomap{

void HashMap::KickToBuffer(ReaderWriterQueue<Item>* q, std::atomic_int& bufferSize){
// sequentially clean all the inactive buckets in the table
    for (int i = 0; i < TABLE_SIZE; i++) {
        // >>1 for all clock items
        clockCounters[i] = clockCounters[i] >> 1;
        // if the bucket is inactive, clean it
        if (clockCounters[i] == 0) {
            if (!table[i].size()) {
                continue;
            }
            // clean the bucket
            for (auto it = table[i].begin(); it != table[i].end(); it++) {
                Item item;
                item.key = it->key;
                item.occupancy = it->occupancy;
                q->enqueue(item);
                bufferSize++;
            }
            // clean the bucket
            table[i].clear();
        }
    }
}


void HashMap::put(const OcTreeKey &key, const bool &value, const uint32_t& hashValue, ReaderWriterQueue<Item>* q) {
    // std::cout << table[hashValue].size() << std::endl;
    // we find from end to begin, if the vector is empty then just skip
    clockCounters[hashValue] = clockCounters[hashValue] | (1 << maxPCNum);
    if (table[hashValue].size()) {
        for (auto it = table[hashValue].end() - 1; it >= table[hashValue].begin(); it--) {
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
    }
    // this node does not exist
    double accumulateOccupancy = 0.0;
    // std::cout << key.k[0] << " " << key.k[1] << " " << key.k[2] << std::endl;
    auto node = tree->search(key);
    // fetch_from_octree++;
    if (node != NULL) {
        accumulateOccupancy = node->getOccupancy();
    }
    // std::cout << "occupancy = " << accumulateOccupancy << std::endl;
    table[hashValue].push_back(HashNode(key, accumulateOccupancy));
    if (table[hashValue].size() > bound) {
        q->enqueue(Item(table[hashValue][0].key, table[hashValue][0].occupancy));
        table[hashValue].erase(table[hashValue].begin());
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

}