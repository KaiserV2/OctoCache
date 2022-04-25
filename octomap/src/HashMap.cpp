#include <octomap/HashMap.h>
#include <octomap/OcTree.h>

namespace octomap{

 // kick key and form item, then put to buffer
void HashMap::KickToBuffer(ReaderWriterQueue<Item>* q, std::atomic_int& bufferSize){
    // void KickToBuffer(std::queue<Item>* q){
#if DEBUG1
        std::cout << "Kicking position " << clock << std::endl;
#endif
        // remove all KV at position clock
        while(table[clock] != NULL) {
            HashNode* entry = table[clock];
            // we find a KV 
            OcTreeKey key = entry->getKey();
            if ((entry->myValue.currentPointCloud = currentPointCloud)) {
                // don't kick that entry because the current stage is updating it
                continue;
            }
            else{
                // first compress that result
                entry->myValue.compress(tree->getProbMissLog(), tree->getProbHitLog());
                // kick that entry
                Item item = Item(key, entry->myValue.accumulateOccupancy);
                q->enqueue(item);
            }
            table[clock] = entry->getNext();
            delete entry;
        }
        clock++;
        if (clock == TABLE_SIZE) {
            clock = 0;
        }
        bufferSize++;
    }

void HashMap::put(const OcTreeKey &key, const bool &value) {
#if DEBUG1
        std::cout << "Putting key into Hash Map" << std::endl;
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
            if (tree->search(key) == NULL) {
                printf("find a NULL\n");
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

}