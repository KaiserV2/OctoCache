#include <octomap/HashMap.h>
#include <octomap/OcTree.h>

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







void HashMap::put(const OcTreeKey &key, const bool &value) {
#if DETAIL_COUNT
        insert_to_hashmap++;
#endif
#if DEBUG2
        if(currentPointCloud == 1){
            std::cout << "Putting key into Hash Map" << std::endl;
        }
#endif

        unsigned long hashValue = MyKeyHash(key);
        
#if DEBUG1
        printf("Hash value is %lu\n", hashValue);
#endif
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

            /* 
             * minghao: The code segment below (line 185-195) might be more efficient if rewritten to be similar to:
            
            OcTreeNode* hashNode = tree->search(key);
            if (hashNode) {
                accumulateOccupancy = hashNode->getOccupancy();
            }
            else{
#if DEBUG1
                printf("find a NULL\n");
#endif
#if DETAIL_COUNT
                fetch_from_octree++;
#endif
                accumulateOccupancy = 0.0;
            }

            Note: I haven't checked the correctness of the piece of code above, but I think it is
                  slightly more efficient.
            
             */

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


void HashMap::cleanHashMap(ReaderWriterQueue<Item>* q, std::atomic_int& bufferSize) {
    currentPointCloud++;
    printf("Cleaning the HashMap\n");
    for (uint32_t i = 0; i < TABLE_SIZE; i++) {
        KickToBuffer(q, bufferSize);
    }
}

}