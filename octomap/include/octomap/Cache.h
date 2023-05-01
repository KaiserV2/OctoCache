#ifndef CACHE
#define CACHE

#include "OcTreeKey.h"
#include "HashMap.h"
#include <iostream>
#include <queue>
#include "OcTreeNode.h"
#include "multi-core/readerwriterqueue.h"
#include <atomic>
#include <thread>
#include <fstream>
#include <string.h>


namespace octomap{

class OcTree; // every function concerned with OcTree only passes OcTree as a pointer

class Cache{
public:
    // have a buffer
    std::atomic_bool run;
    std::atomic_int bufferSize;
    ReaderWriterQueue<std::pair<OcTreeKey,double>> buffer;
    // std::queue<Item> buffer;
    HashMap myHashMap;
    OcTree* tree;
    std::thread thd;
    uint32_t pktCount;
    uint32_t clockWait = 16;
    bool runThread;
    double inOutRatio;
    uint32_t evictNum;
    uint32_t bound;
    // function to feed items in the buffer to the octree

    Cache(uint32_t _tableSize, uint32_t _tableWidth, OcTree* _tree, uint32_t _clockWait);
    ~Cache();
    
    void ProcessPkt(const OcTreeKey &key, const bool &value);
    void Kick();
    // static void DigestBuffer(std::thread* thisThd);
    void EndThread();
    void EndOneThread();
    void StartThread();
    void PrintBuffer();
    void test();
    void adjust(uint32_t PCSize);
    double search(const OcTreeKey &key);
    double search(double x, double y, double z);
    void waitForEmptyBuffer();
    // void test(AbstractOcTree* tree);

};

}
#endif

