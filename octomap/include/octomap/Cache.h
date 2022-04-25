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
    std::atomic_int bufferSize;
    ReaderWriterQueue<Item> buffer;
    // std::queue<Item> buffer;
    HashMap myHashMap;
    OcTree* tree;
    std::thread thd;
    uint32_t pktCount;
    uint32_t clockWait;
    // function to feed items in the buffer to the octree

    Cache(uint32_t _TABLE_SIZE, uint32_t _clockWait) {
        myHashMap.init(_TABLE_SIZE, _clockWait);
        bufferSize = 0;
        pktCount = 0; // here pkt count means the number of "duplicated insertions"
        clockWait = 16; // make it 2^n, the default is 90k / 7k
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

