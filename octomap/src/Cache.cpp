#include <octomap/Cache.h>
#include <octomap/OcTree.h>

namespace octomap{
    void Cache::DigestBuffer() {
        while (buffer.size() > 0) {
            Item item = buffer.front();
            buffer.pop();
            OcTreeKey key = item.key;
            MyQueue myQueue = item.myQueue;
            // digest a single item, containing several updates of one voxel to the octree
            while (myQueue.dq.size() > 0) {
                bool occupancy = myQueue.dq.front().occupancyCount;
                if (occupancy == true) {
                    // its an occupied voxel
                    tree->updateNode(key, true, lazy_eval);
                }
                else {
                    // a free voxel
                    tree->updateNode(key, false, lazy_eval);
                }
            }
        }
    }

    void PrintBuffer() {
        for (auto it = buffer.begin(); it != buffer.end(); it++) {
            it->PrintItem();
        }
    }

    void Cache::test(){
        tree->test();
    }
}