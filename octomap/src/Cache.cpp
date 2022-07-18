#include <octomap/Cache.h>
#include <octomap/OcTree.h>
#include <pthread.h>
#include <thread>
#include <sys/time.h>
// #include <x86intrin.h>

#define DEBUG3 false 

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
        // uint64_t point1, point2, point3, point4;
        // point1 = __rdtsc();
        uint32_t hashValue = this->myHashMap.ScalarHash(key, value);
        // point2 = __rdtsc();
        this->myHashMap.put(key, value, hashValue % this->myHashMap.TABLE_SIZE);
        // point3 = __rdtsc();
        pktCount++;
        if (pktCount % clockWait == 0) {
#if ONE_THREAD
            // directly kick from hash map to octree
            this->myHashMap.KickToOctree();
#else
            this->myHashMap.KickToBuffer(&buffer, bufferSize);
            // point4 = __rdtsc();
            // kick_time +=  point4 - point3;
#endif
        }
        // hash_time +=  point2 - point1;
        // put_time +=  point3 - point2;
    }

    void DigestBuffer(std::thread* thisThd, Cache* myCache) {
#ifdef __linux__
        if(!setaffinity(thisThd, 20))
            return;
#endif
        while((myCache->run) || (myCache->bufferSize != 0)) {
            Item item;
            if (myCache->run == false){
                std::cout << "BufferSize returns" << myCache->bufferSize << std::endl;
                std::cout << "Trydequeue returns" << myCache->buffer.try_dequeue(item) << std::endl;
                break;
            }
            while (myCache->buffer.try_dequeue(item)) { 
#if DEBUG1
                std::cout << "Putting item to octree!" << std::endl;
#endif
                OcTreeKey key = item.key;
                bool occupancy = item.occupancy;
<<<<<<< HEAD
                // fout << key.k[0] << " " << key.k[1] << " " << key.k[2] << occupancy << std::endl;
=======
                myCache->fout << key.k[0] << " " << key.k[1] << " " << key.k[2] << " " << occupancy << std::endl;
>>>>>>> 264d64c90dd78e2c6d98e2a75e1f6f03b635a6e8
                if (occupancy == true) {
                    // its an occupied voxel
                    myCache->tree->updateNode(key, true, lazy_eval);
                    myCache->bufferSize--;
                    // std::cout << myCache->bufferSize << std::endl;
#if DEBUG1
                    std::cout << "Done octree insertion" << std::endl;
#endif
                }
                else {
                    // a free voxel
                    myCache->tree->updateNode(key, false, lazy_eval);
                    myCache->bufferSize--;
                    // std::cout << myCache->bufferSize << std::endl;
#if DEBUG1
                    std::cout << "Done octree insertion" << std::endl;
#endif
                }
#if DETAIL_COUNT
                    insert_to_octree++;
#endif
            }
        }
    }

    void OneDigestBuffer(Cache* myCache) {
        while((myCache->run) || (myCache->bufferSize != 0)) {
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
                    myCache->bufferSize--;
                    // std::cout << myCache->bufferSize << std::endl;
#if DEBUG1
                    std::cout << "Done octree insertion" << std::endl;
#endif
                }
                else {
                    // a free voxel
                    myCache->tree->updateNode(key, false, lazy_eval);
                    myCache->bufferSize--;
                    // std::cout << myCache->bufferSize << std::endl;
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
        // this->myHashMap.cleanHashMap(&buffer, bufferSize);
        this->run = false; 
        thd.join();
        std::cout << "Remaining buffersize" << bufferSize << std::endl;
        // Item item;
        // std::cout << "Buffer check: " <<  this->buffer.try_dequeue(item) << std::endl;
    }

    void Cache::EndOneThread() { // test for single thread
        timeval start; 
        timeval stop; 
        gettimeofday(&start, NULL);  // stop timer
        // std::cout << "Before buffersize" << bufferSize << std::endl;
        int start_bufferSize = bufferSize;
        this->myHashMap.cleanHashMap(&buffer, bufferSize);
        gettimeofday(&stop, NULL);  // stop timer
        double time_to_insert = (stop.tv_sec - start.tv_sec) + 1.0e-6 *(stop.tv_usec - start.tv_usec);
        std::cout <<  "Insertion to buffer time " << time_to_insert << " sec" << std::endl;
        this->run = false; 
        // std::cout << "After buffersize" << bufferSize << std::endl;
        int end_bufferSize = bufferSize;
        std::cout << "How many items " << end_bufferSize - start_bufferSize << std::endl;
        std::cout << "Per item time " << time_to_insert / (end_bufferSize - start_bufferSize) << std::endl;
        //OneDigestBuffer(this);
        // Item item;
        // std::cout << "Buffer check: " <<  this->buffer.try_dequeue(item) << std::endl;
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