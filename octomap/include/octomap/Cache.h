#ifndef CACHE
#define CACHE

#include "OcTreeKey.h"
#include "HashMap.h"
#include <iostream>
#include <queue>
#include "GlobalVariables_Octomap.h"
#include "OcTreeNode.h"
#include "multi-core/readerwriterqueue.h"
#include <atomic>
#include <thread>

namespace octomap{

class OcTree; // every function concerned with OcTree only passes OcTree as a pointer

class Cache{
public:
    // have a buffer
    std::atomic_bool run;
    ReaderWriterQueue<Item> buffer;
    // std::queue<Item> buffer;
    HashMap myHashMap;
    OcTree* tree;
    std::thread thd;
    // function to feed items in the buffer to the octree

    Cache(uint32_t _TABLE_SIZE) {
        myHashMap.init(_TABLE_SIZE);
    } 
    ~Cache() {}
    
    void ProcessPkt(const OcTreeKey &key, const bool &value);
    // static void DigestBuffer(std::thread* thisThd);
    void EndThread();
    void StartThread();
    void PrintBuffer();
    void test();
    // void test(AbstractOcTree* tree);

};

}
#endif

