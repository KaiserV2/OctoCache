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
#include <fstream>
#include <string.h>


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
    bool runThread;
    double inOutRatio;
    uint32_t evictNum;
    uint32_t bound;

    double lastVoxelInsertTime;
    int interval; // the interval between 2 adjustments
    bool increaseFlag;
    // function to feed items in the buffer to the octree

    Cache(uint32_t _TABLE_SIZE, OcTree* _tree, double _bound = 10, int _maxPCNum = 10) {
        myHashMap.init(_TABLE_SIZE, _tree, _bound, _maxPCNum);
        bufferSize = 0;
        tree = _tree;
        pktCount = 0; // here pkt count means the number of "duplicated insertions"
        runThread = true;
        bound = _bound;
        lastVoxelInsertTime = 0;
        interval = _maxPCNum + 1;
        increaseFlag = false;
    } 
    ~Cache() {
        // fout.close();
    }
    
    void ProcessPkt(const OcTreeKey &key, const bool &value);
    void Kick();
    // static void DigestBuffer(std::thread* thisThd);
    void EndThread();
    void EndOneThread();
    void StartThread();
    void PrintBuffer();
    void test();
    void adjust(double perVoxelInsertTime);
    // void test(AbstractOcTree* tree);

};

}
#endif

