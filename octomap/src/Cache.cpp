#include <octomap/Cache.h>
#include <octomap/OcTree.h>
#include <octomap/GlobalVariables_Octomap.h>
#include <octomap/GlobalVariables_Cache.h>
#include <pthread.h>
#include <thread>
#include <sys/time.h>
#ifdef __x86_64__
#include <x86intrin.h> // for rdtsc on x86
#elif __aarch64__
#include <stdint.h> // for uint64_t on ARM64
#endif


#define CPU_CYCLES false

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

template <class Tree, class NODE>
    Cache<Tree, NODE>::Cache(uint32_t _tableSize, uint32_t _tableWidth, Tree* _tree, uint32_t _clockWait) {
        std::cout << "Initializing cache" << std::endl;
        myHashMap.init(_tableSize, _tableWidth, _tree);
        bufferSize = 0;
        insert_to_octree = 0;
        tree = _tree;
        pktCount = 0; // here pkt count means the number of "duplicated insertions"
        clockWait = _clockWait; // make it 2^n, the default is 90k / 7k
        runThread = true;
        // fout.open(file);
        inOutRatio = 0.0;
        this->StartThread();
    }

    // Cache::~Cache(){
    //     print_time("end");
    //     this->EndThread();
    // }

template <class Tree, class NODE>
    void Cache<Tree, NODE>::updateNode(const OcTreeKey& key, bool occupied, bool lazy_eval){
#if CPU_CYCLES
        uint64_t point1, point2, point3, point4;
#endif
        // std::cout << "[" << key.k[0] << "," << key.k[1] << "," << key.k[2] << "]," << std::endl;
        // uint32_t hashValue = this->myHashMap.ScalarHash(key);
        uint32_t hashValue = this->myHashMap.MortonHash(key);
#if CPU_CYCLES
#endif
        this->myHashMap.put(key, occupied, hashValue, &buffer, bufferSize);
        // std::cout << 2 << std::endl;
#if CPU_CYCLES
#endif
        return; // we do the eviction outside
        pktCount++;
        if (pktCount % clockWait == 0) {
            this->myHashMap.KickToBuffer(&buffer, bufferSize);
        }
    }

template <class Tree, class NODE>
    void Cache<Tree, NODE>::Kick() {
        myHashMap.KickToBuffer(&buffer, bufferSize);
        return;
    }

template <class Tree, class NODE>
    void DigestBuffer(std::thread* thisThd, Cache<Tree, NODE>* myCache) {
// #ifdef __linux__
//         if(!setaffinity(thisThd, 5))
//             return;
// #endif
        while (true) {
            if (!myCache->run) {
                break;
            }
            if (myCache->bufferSize){
                myCache->mtx.lock();
                // std::cout << "unlock" << std::endl;
                while(myCache->bufferSize){
                    std::pair<OcTreeKey,double> pair;
                    if (myCache->buffer.try_dequeue(pair)) {
                        // mtx.lock();
                        myCache->tree->setNodeValue(pair.first, pair.second, lazy_eval);
                        myCache->bufferSize--;
                        // mtx.unlock();
                        myCache->insert_to_octree++;
                    }
                }
                myCache->mtx.unlock();
            }
        }
    }

template <class Tree, class NODE>
    void OneDigestBuffer(Cache<Tree, NODE>* myCache) {
        while((myCache->run) || (myCache->bufferSize != 0)) {
            std::pair<OcTreeKey,double> pair;
            while (myCache->buffer.try_dequeue(pair)) { 
#if DEBUG1
                std::cout << "Putting item to octree!" << std::endl;
#endif
                if (pair.second == true) {
                    // its an occupied voxel
                    myCache->tree->updateNode(pair.first, true, lazy_eval);
                    myCache->bufferSize--;
                    // std::cout << myCache->bufferSize << std::endl;
#if DEBUG1
                    std::cout << "Done octree insertion" << std::endl;
#endif
                }
                else {
                    // a free voxel
                    myCache->tree->updateNode(pair.first, false, lazy_eval);
                    myCache->bufferSize--;
                    // std::cout << myCache->bufferSize << std::endl;
#if DEBUG1
                    std::cout << "Done octree insertion" << std::endl;
#endif
                }
            }
        }
    }

template <class Tree, class NODE>
    void Cache<Tree, NODE>::StartThread() {
        this->run = true;
#if DEBUG1
        std::cout << "Starting the thread" << std::endl;
#endif
        thd = std::thread(DigestBuffer, &thd, this);
    }

template <class Tree, class NODE>
    void Cache<Tree, NODE>::EndThread() {
        if (this->run == false) {
            return;
        }
        std::cout << "We turn off the thread" << std::endl;
        // this->myHashMap.cleanHashMap(&buffer, bufferSize);
        this->run = false; 
        thd.join();
        std::cout << "Remaining buffersize " << bufferSize << std::endl;
        std::cout << "Total insertion to Octree " << insert_to_octree << std::endl;
        // Item item;
        // std::cout << "Buffer check: " <<  this->buffer.try_dequeue(item) << std::endl;
    }

template <class Tree, class NODE>
    void Cache<Tree, NODE>::EndOneThread() { // test for single thread
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

template <class Tree, class NODE>
    void Cache<Tree, NODE>::PrintBuffer() {
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

template <class Tree, class NODE>
    void Cache<Tree, NODE>::test(){
        tree->test();
    }

template <class Tree, class NODE>
    NODE* Cache<Tree, NODE>::search(const OcTreeKey &key, unsigned int depth) {
        NODE* node = myHashMap.get(key);
        return node;
    }

template <class Tree, class NODE>
    NODE* Cache<Tree, NODE>::search(const point3d& value, unsigned int depth) { // remember to delete the node after return
        OcTreeKey key;
        if (!tree->coordToKeyChecked(value, key)){
            OCTOMAP_ERROR_STR("Error in search: ["<< value <<"] is out of OcTree bounds!");
            return NULL;
        }
        else {
            return search(key, depth);
        }
    }

template <class Tree, class NODE>
    NODE* Cache<Tree, NODE>::search(double x, double y, double z, unsigned int depth) {
        // first convert into OcTreeKey
        OcTreeKey key;
        if (!tree->coordToKeyChecked(x, y, z, key)){ // if the coordinates does not correspond to a valid key
            OCTOMAP_ERROR_STR("Error in search: ["<< x <<" "<< y << " " << z << "] is out of OcTree bounds!");
            return NULL;
        }
        else {
            return search(key);
        }
    }

template <class Tree, class NODE>
    void Cache<Tree, NODE>::waitForEmptyBuffer() {
        while(bufferSize) {}
    }

template <class Tree, class NODE>
    void Cache<Tree, NODE>::flush() {
        
    }

template <class Tree, class NODE>
    void Cache<Tree, NODE>::printInfo() {
        printf("fetch from octree: %d\n", fetch_from_octree);
        fetch_from_octree = 0;
    }
}