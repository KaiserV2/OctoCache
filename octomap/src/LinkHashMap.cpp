#include <octomap/HashMap.h>
#include <octomap/OcTree.h>

#define DEBUG3 false

namespace octomap{

 // kick key and form item, then put to buffer
void HashMap::KickToBuffer(ReaderWriterQueue<Item>* q, std::atomic_int& bufferSize){
    // void KickToBuffer(std::queue<Item>* q){
#if DEBUG2
    if (currentPointCloud == 1){
        std::cout << "Kicking position " << clock << std::endl;
    }
#endif
        // remove all KV at position clock
        HashNode *prev = NULL;
        HashNode* entry = table[clock];
        while(entry != NULL) {
            // we find a KV 
#if DEBUG2
            if (currentPointCloud == 1){
                printf("visited point cloud is %d\n", entry->myValue.currentPointCloud);
            }  
#endif
            OcTreeKey key = entry->getKey();
            if ((entry->myValue.currentPointCloud == currentPointCloud)) {
                // don't kick that entry because the current stage is updating it
                prev = entry;
                entry = entry->getNext();
                continue;
            }
            else{
                // first compress that result
                entry->myValue.compress(tree->getProbMissLog(), tree->getProbHitLog());
                // kick that entry
                Item item = Item(key, entry->myValue.accumulateOccupancy);
                q->enqueue(item);
#if DETAIL_COUNT
                insert_to_buffer++;
#endif
                bufferSize++;
                // itemCount++;
                if (prev == NULL) {
                    // entry is the first
                    table[clock] = entry->getNext();
                    delete entry;
                    entry = table[clock];
                }
                else{
                    prev->setNext(entry->getNext());
                    delete entry;
                    entry = prev->getNext();
                }   
#if DEBUG2
                if (currentPointCloud == 1){
                    printf("get here\n");
                }  
#endif
            }
        }
        clock++;
        if (clock == TABLE_SIZE) {
            clock = 0;
        }
#if DEBUG2
        if (currentPointCloud == 1){
            std::cout << "Finished kicking" << std::endl;
        }
#endif
    }






void HashMap::KickToOctree() {
// remove all KV at position clock
    HashNode *prev = NULL;
    HashNode* entry = table[clock];
    while(entry != NULL) {
        // we find a KV 
#if DEBUG2
        if (currentPointCloud == 1){
            printf("visited point cloud is %d\n", entry->myValue.currentPointCloud);
        }  
#endif
        OcTreeKey key = entry->getKey();
        if ((entry->myValue.currentPointCloud == currentPointCloud)) {
            // don't kick that entry because the current stage is updating it
            prev = entry;
            entry = entry->getNext();
            continue;
        }
        else{
            // first compress that result
            entry->myValue.compress(tree->getProbMissLog(), tree->getProbHitLog());
            // kick that entry
            Item item = Item(key, entry->myValue.accumulateOccupancy);
            tree->updateNode(key, item.occupancy, lazy_eval);
            if (prev == NULL) {
                // entry is the first
                table[clock] = entry->getNext();
                delete entry;
                entry = table[clock];
            }
            else{
                prev->setNext(entry->getNext());
                delete entry;
                entry = prev->getNext();
            }   
        }
    }
    clock++;
    if (clock == TABLE_SIZE) {
        clock = 0;
    }
#if DEBUG2
    if (currentPointCloud == 1){
        std::cout << "Finished kicking" << std::endl;
    }
#endif
}

// void HashMap::put(const OcTreeKey &key, const bool &value){
void HashMap::put(const OcTreeKey &key, const bool &value, const uint32_t& hashValue) {
#if DETAIL_COUNT
        insert_to_hashmap++;
#endif
        // unsigned long hashValue = MyKeyHash(key);
        HashNode *prev = NULL;
        HashNode *entry = table[hashValue];

        while (entry != NULL && entry->getKey() != key) {
            prev = entry;
            entry = entry->getNext();
        }

        if (entry == NULL) {
#if DEBUG1
            printf("entry is NULL\n");
#endif            
            // create new hash node
            // first query the key into Octree
            double accumulateOccupancy;
            assert(tree != nullptr);
            if (tree->search(key) == NULL) {
#if DEBUG1
                printf("find a NULL\n");
#endif
#if DETAIL_COUNT
                    fetch_from_octree++;
#endif
                accumulateOccupancy = 0.0;
            }
            else{
                accumulateOccupancy = tree->search(key)->getOccupancy();
            }
            // then create MyValue
#if DEBUG1
            printf("%f,%d\n", accumulateOccupancy, currentPointCloud);
#endif
            MyValue myValue = MyValue(accumulateOccupancy, value, currentPointCloud);

            entry = new HashNode(key, myValue);
            if (prev == NULL) {
                // insert as first bucket
                table[hashValue] = entry;
            } else {
                prev->setNext(entry);
            }
        } else {
            // just update the value
            if (entry->myValue.currentPointCloud == currentPointCloud) {
                // updating in the current point cloud
                entry->myValue.currentOccupancy = entry->myValue.currentOccupancy || value;
            }
            else{
                // comes in with a new point cloud
                // first merge information of the last round
                entry->myValue.compress(tree->getProbMissLog(), tree->getProbHitLog());
                // then set up for new point cloud statistics
                entry->myValue.currentPointCloud = currentPointCloud;
                entry->myValue.currentOccupancy = value;
            }
        }
#if DEBUG1
        printf("Successfully put key\n");
#endif        
    }



uint32_t HashMap::ScalarHash(const OcTreeKey &key, const bool &value){
    uint32_t keys[2] = {0, 0};
    memcpy((uint16_t*)keys, &key.k[0], 6);
    uint32_t hashValue = murmur3<2>::scalar((uint32_t *)(&(keys[0])), hashSeed);
    // uint32_t hashValue = hashFunc.run((const char *)(&(key.k[0])), sizeof(key_type) * 3) % TABLE_SIZE;
    return hashValue;
}



uint32_t HashMap::MortonHash(const OcTreeKey &key, const bool &value){
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