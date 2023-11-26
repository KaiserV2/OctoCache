#include <octomap/HashMap.h>
#include <octomap/OcTree.h>
#include <octomap/GlobalVariables_Octomap.h>
#include <octomap/GlobalVariables_Cache.h>
#include <algorithm>

#define DEBUG3 true

namespace octomap{

 
OcTreeNode* HashMap::get(const OcTreeKey &key) {
    // this function waits to be finished
    // search each bucket after another, stops if found 10 consecutive full buckets
    unsigned long hashValue = MortonHash(key);
#if USE_CQ
    int i = table[hashValue].find(key);
    if (i != -1) {
        return table[hashValue].getValue(i);
    }    
#else
    for (auto it = table[hashValue].begin(); it != table[hashValue].end(); it++) {
        if (it->first == key) {
            return it->second;
        }
    }
#endif
    // it is impossible to return a so large number as 100, note as not found
    return NULL;
}

 
void HashMap::KickToBuffer(ReaderWriterQueue<std::pair<OcTreeKey,float>>* q, std::atomic_int& bufferSize){
// sequentially clean all the inactive buckets in the table
//     for (int i = 0; i < TABLE_SIZE; i++) {
//         // >>1 for all clock items
//         clockCounters[i] = clockCounters[i] >> 1;
//         // if the bucket is inactive, clean it
//         if (clockCounters[i] == 0) {
// #if USE_CQ
//             if (table[i].count() == 0) { // no items in the circular queue
//                 continue;
//             }
//             // clean the bucket
//             for (int j = table[i].back(); j != table[i].front(); j = (j + table[i].maxitems()) % (table[i].maxitems() + 1)) {
//                 q->enqueue(std::make_pair(table[i].getKey(j), table[i].getValue(j)->getLogOdds()));
//                 bufferSize++;
//                 delete table[i].getValue(j);
//                 table[i].getValue(j) = nullptr;
//             }
//             // clean the bucket
//             table[i].clear();
// #endif
//         }
//     }
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (table[i].size() > bound) {
            // kick from the front to the back, until the size is smaller than the bound
            while (table[i].size() > bound) {
                q->enqueue(std::make_pair(table[i].front().first, table[i].front().second->getLogOdds()));
                bufferSize++;
                delete table[i].front().second;
                // table[i].front().second = nullptr;
                table[i].pop_front();
            }
        }
    }
}

 
void HashMap::put(const OcTreeKey &key, const bool &value, const uint32_t& hashValue, ReaderWriterQueue<std::pair<OcTreeKey,float>>* q, std::atomic_int& bufferSize) {
#if DETAIL_COUNT
    insert_to_hashmap++;
#endif
#if USE_CQ
    // tree->setMinMax(key);
    // std::cout << TABLE_SIZE << " " << hashValue << std::endl;
    // clockCounters[hashValue] = clockCounters[hashValue] | 128;
    int i = table[hashValue].find(key);
    if (i != -1) { 
        // std::cout << "found" << std::endl;
        if (value == true) { // occupied cells
            float temp = table[hashValue].getValue(i)->getLogOdds() + tree->getProbHitLog();
            table[hashValue].getValue(i)->setLogOdds(std::min(temp, tree->getClampingThresMaxLog()));
            // table[hashValue].replace(i, std::min(temp, tree->getClampingThresMaxLog()));
            return;
        }
        else {
            float temp = table[hashValue].getValue(i)->getLogOdds() + tree->getProbMissLog();
            table[hashValue].getValue(i)->setLogOdds(std::max(temp, tree->getClampingThresMinLog()));
            // table[hashValue].replace(i, std::max(temp, tree->getClampingThresMinLog()));
            return;
        }
    }
#else
    for (auto it = table[hashValue].begin(); it != table[hashValue].end(); it++) {
        if (it->first == key){
            // std::cout << "found" << std::endl;
            if (value == true) {
                it->second->setLogOdds(std::min(it->second->getLogOdds() + tree->getProbHitLog(), tree->getClampingThresMaxLog()));
                return;
            }
            else {
                it->second->setLogOdds(std::max(it->second->getLogOdds() + tree->getProbMissLog(), tree->getClampingThresMinLog()));
                return;
            }
        }
    }
#endif
    // this node does not exist
    double accumulateOccupancy = 0.0;
    auto node = tree->search(key);
    fetch_from_octree++;
    if (node != NULL) {
        accumulateOccupancy = node->getLogOdds();
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
#if USE_CQ
    if (table[hashValue].isFull()) {
        q->enqueue(std::make_pair(table[hashValue].last().first, table[hashValue].last().second->getLogOdds()));
        bufferSize++;
        table[hashValue].pop();
    }
    OcTreeNode* newNode = new OcTreeNode();
    newNode->setLogOdds(accumulateOccupancy);
    table[hashValue].push(key, newNode);
#else
    OcTreeNode* newNode = new OcTreeNode();
    newNode->setLogOdds(accumulateOccupancy);
    table[hashValue].push_back(std::make_pair(key, newNode));
#endif
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

 
HashMap::~HashMap() {
    delete[] table;
    // delete[] clockCounters;
}

 
void HashMap::cleanHashMap(ReaderWriterQueue<std::pair<OcTreeKey,float>>* q, std::atomic_int& bufferSize) {
    currentPointCloud++;
    printf("Cleaning the HashMap\n");
    for (uint32_t i = 0; i < TABLE_SIZE; i++) {
        KickToBuffer(q, bufferSize);
    }
}

 
int HashMap::loadSize(){
    int sum = 0;
    for (int i = 0; i < TABLE_SIZE; i++) {
#if USE_CQ
        sum += table[i].count();
#else
        sum += table[i].size();
#endif
    }
    return sum;
}

}