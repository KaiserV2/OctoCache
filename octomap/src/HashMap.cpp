#include <octomap/HashMap.h>
#include <octomap/OcTree.h>

namespace octomap{

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
            // create new hash node
            // first query the key into Octree
            double accumulateOccupancy = tree->search(key)->getOccupancy();
            // then create MyValue
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
                entry->myValue->currentOccupancy = entry->myValue->currentOccupancy || value;
            }
            else{
                // comes in with a new point cloud
                // first merge information of the last round
                if (entry->myValue->currentOccupancy == false) {
                    entry->myValue->accumulateOccupancy += tree->prob_miss_log;
                }
                else{
                    entry->myValue->accumulateOccupancy += tree->prob_hit_log;
                }
                // then set up for new point cloud statistics
                entry->myValue->currentPointCloud = currentPointCloud;
                entry->myValue->currentOccupancy = value;
            }
            entry->setValue(myValue); // change here
        }
        // end of put KV, move the clock
    }

}