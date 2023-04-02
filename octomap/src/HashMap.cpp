#include <octomap/HashMap.h>
#include <octomap/OcTree.h>
#include <octomap/GlobalVariables_Octomap.h>
#include <octomap/GlobalVariables_Cache.h>

#define DEBUG3 true

namespace octomap{

void HashMap::KickToBuffer(ReaderWriterQueue<std::pair<OcTreeKey,double>>* q, std::atomic_int& bufferSize){
// sequentially clean all the inactive buckets in the table
    for (int i = 0; i < TABLE_SIZE; i++) {
        // >>1 for all clock items
        clockCounters[i] = clockCounters[i] >> 1;
        // if the bucket is inactive, clean it
        if (clockCounters[i] == 0) {
            if (table[i].count() == 0) { // no items in the circular queue
                continue;
            }
            // clean the bucket
            for (int j = table[i].back(); j != table[i].front(); j = (j + table[i].maxitems()) % (table[i].maxitems() + 1)) {
                // if the item is not in the current point cloud, kick it to the buffer
                q->enqueue(table[i].getPair(j));
                bufferSize++;
            }
            // clean the bucket
            table[i].clear();
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
            return;
        }
        else {
            table[hashValue].getValue(pos) += tree->getProbMissLog();
            if (table[hashValue].getValue(pos) < tree->getClampingThresMinLog()) {
                table[hashValue].getValue(pos) = tree->getClampingThresMinLog();
            }
            return;
        }
    }
    // this node does not exist
    double accumulateOccupancy = 0.0;
    auto node = tree->search(key);
    // fetch_from_octree++;
    if (node != NULL) {
        accumulateOccupancy = node->getOccupancy();
    }
    if (value == true) {
        accumulateOccupancy += tree->getProbHitLog();
        if (accumulateOccupancy > tree->getClampingThresMaxLog()) {
            accumulateOccupancy = tree->getClampingThresMaxLog();
        }
    }
    else {
        accumulateOccupancy += tree->getProbMissLog();
        if (accumulateOccupancy < tree->getClampingThresMinLog()) {
            accumulateOccupancy = tree->getClampingThresMinLog();
        }
    }
    if (table[hashValue].isFull()) {
        q->enqueue(table[hashValue].last());
        table[hashValue].pop();
    }
    table[hashValue].push(key, accumulateOccupancy);
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


void HashMap::cleanHashMap(ReaderWriterQueue<std::pair<OcTreeKey,double>>* q, std::atomic_int& bufferSize) {
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