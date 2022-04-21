#include <octomap/Cache.h>
#include <octomap/OcTree.h>
#include <pthread.h>
#include <thread>

namespace octomap{

#ifdef __linux__
    static bool setaffinity(std::thread* thd, uint32_t coreId){
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(coreId, &cpuset);
        int rc = pthread_setaffinity_np(thd->native_handle(),
                                        sizeof(cpu_set_t), &cpuset);
        if (rc != 0) {
            std::cerr << "Error calling pthread_setaffinity_np: " << rc << "\n";
            return false;
        }
        return true;
    }
#endif

    void Cache::ProcessPkt(const OcTreeKey &key, const bool &value){
#if DEBUG1
        std::cout << "Processing packet" << std::endl;
#endif
        this->myHashMap.put(key, value);
        this->myHashMap.KickToBuffer(&buffer);
    }

    void DigestBuffer(std::thread* thisThd, Cache* myCache) {
#ifdef __linux__
        if(!setaffinity(thisThd, 20))
            return;
#endif
#if DEBUG1
        std::cout << "Digesting Buffer " << std::endl;
#endif
        while(myCache->run) {
            Item item;
            while (myCache->buffer.try_dequeue(item)) { 
#if DEBUG1
                std::cout << "Putting item to octree!" << std::endl;
#endif
                OcTreeKey key = item.key;
                bool occupancy = item.occupancy;
                if (occupancy == true) {
                    // its an occupied voxel
                    myCache->tree->updateNode(key, true, lazy_eval);
#if DEBUG1
                    std::cout << "Done octree insertion" << std::endl;
#endif
                }
                else {
                    // a free voxel
                    myCache->tree->updateNode(key, false, lazy_eval);
#if DEBUG1
                    std::cout << "Done octree insertion" << std::endl;
#endif
                }
            }
        }
    }

    void Cache::StartThread() {
        this->run = true;
#if DEBUG1
        std::cout << "Starting the thread" << std::endl;
#endif
        thd = std::thread(DigestBuffer, &thd, this);
    }

    void Cache::EndThread() {
        std::cout << "We turn off the thread" << std::endl;
        this->myHashMap.cleanHashMap(&buffer);
        this->run = false;
        thd.join();
        Item item;
        bool ans = this->buffer.try_dequeue(item);
        printf("Buffer check: ");
        std::cout << this->buffer.try_dequeue(item) << std::endl;
    }

    void Cache::PrintBuffer() {
#if DEBUG1
        std::cout << "Printing buffer" << std::endl;
#endif
        // std::queue<Item> tmpBuffer = buffer;
        // while(tmpBuffer.size() > 0) {
        //     Item item = tmpBuffer.front();
        //     item.PrintItem();
        //     tmpBuffer.pop();
        // }
    }

    void Cache::test(){
        tree->test();
    }
}