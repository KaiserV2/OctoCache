#ifndef HASH_MAP_H
#define HASH_MAP_H

#include "Param.h"
#include "BOBHash32.h"
#include <queue>
#include <vector>
#include <atomic>
#include <iostream>
#include <bitset>
#include <string.h>
#include "OcTreeKey.h"
#include "OcTreeNode.h"
#include "data-structure/CircularQueue.h"
#include "multi-core/readerwriterqueue.h"
#include "hash/parallel-murmur3.h"
#include "hash/parallel-xxhash.h"

#define USE_CQ true // true for using circular queue, false for using vector (not completed)

namespace octomap{


class OcTree;

extern int pointCloudCount;

class MyPair {
public:
    int pointCloudNum;
    bool occupancyCount; // + means occupied, - means free
    MyPair() {}
    MyPair(int p, bool o) {
        pointCloudNum = p;
        occupancyCount = o;
    }
};

class OcTreeKeyValuePair {
public:
    OcTreeKey key;
    bool value;
    OcTreeKeyValuePair(){}
    OcTreeKeyValuePair(const OcTreeKey& _key, const bool& _value){
        key = _key;
        value = _value;
    }
    OcTreeKeyValuePair(const OcTreeKeyValuePair& p) {
        key = p.key;
        value = p.value;
    }
};


class MyQueue {
public:
    std::deque<MyPair> dq;
    MyQueue(){}
    MyQueue(const MyQueue& mq){
        this->dq = mq.dq;
    }
    void Update(int _pcNum, bool val) {
#if DETAIL_LOG
        std::cout << "Updating from point cloud " << _pcNum << " with value " << val << std::endl;
#endif
        int pcNum = dq.back().pointCloudNum;
        if (pcNum == _pcNum) {
            // update in the same point cloud
            dq.back().occupancyCount |= val;
        }
        else {
            // a new point cloud starts
            MyPair mp = MyPair(_pcNum, val);
            dq.push_back(mp);
        }
        return;
    }

};

class Item{
public:
    OcTreeKey key;
    float occupancy;
    Item(){}
    Item(const OcTreeKey& _key, const double& _occupancy){
        key = _key;
        occupancy = _occupancy;
    }
    Item(const Item & item){
        key = item.key;
        occupancy = item.occupancy;
    }
    ~Item(){}
    void PrintItem() {
        std::cout << "Key: (" << key.k[0] << ','  << key.k[1] << ','  << key.k[2] << ')';
        std::cout << ", Value: " << occupancy << std::endl;
    }
};



// Hash node class template
class HashNode {
public:
    HashNode(const OcTreeKey &_key, const double& _occupancy) {
        key = _key;
        occupancy = _occupancy;
    }

    HashNode(){}


    OcTreeKey getKey() const {
        return key;
    }

public:
    OcTreeKey key;
    double occupancy;
};


// Hash map class template
template <class NODE>
class HashMap {
public:

    HashMap(){

    }

    void init(uint32_t _TABLE_SIZE, uint32_t _bound, OcTree* _tree) {
        // construct zero initialized hash table of size
        TABLE_SIZE = _TABLE_SIZE;
        // table = new std::vector<HashNode>[TABLE_SIZE];
        table = new CircularQueue<OcTreeKey, NODE*>[TABLE_SIZE];
        for (int i = 0; i < TABLE_SIZE; i++) {
            table[i].init(_bound);
        }
        clockCounters = new uint8_t[TABLE_SIZE];
        memset(clockCounters, 0, TABLE_SIZE);
        currentPointCloud = 0;
        tree = _tree;
        bound = _bound;
    }

    ~HashMap();

    NODE* get(const OcTreeKey &key);

    void KickToBuffer(ReaderWriterQueue<std::pair<OcTreeKey,float>>* q, std::atomic_int& bufferSize);

    void put(const OcTreeKey &key, const bool &value, const uint32_t& hashValue, ReaderWriterQueue<std::pair<OcTreeKey,float>>* q, std::atomic_int& bufferSize);

    uint32_t ScalarHash(const OcTreeKey &key);

    uint32_t MortonHash(const OcTreeKey &key);

    uint32_t RoundRobin(uint32_t count);

    // when the whole workflow ends, clean all the items that are stalk within the cache
    void cleanHashMap(ReaderWriterQueue<std::pair<OcTreeKey,float>>* q, std::atomic_int& bufferSize);

    void KickToOctree();

    void Kick(int num, ReaderWriterQueue<std::pair<OcTreeKey,float>>* q, std::atomic_int& bufferSize);

    int loadSize();

public:
    // hash table
    // std::vector<HashNode> *table;
#if USE_CQ
    CircularQueue<OcTreeKey, NODE*> *table;
#else
    std::vector<HashNode> *table;
#endif
    uint8_t* clockCounters; // the maximum number of point clouds in the cache is 7 (if >=8, change into a uint16_t...)
    uint32_t TABLE_SIZE;
    OcTree* tree;
    uint32_t currentPointCloud;
    uint32_t bound = 12;
};

}

#endif