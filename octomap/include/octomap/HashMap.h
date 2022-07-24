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
#include "multi-core/readerwriterqueue.h"
#include "hash/parallel-murmur3.h"
#include "hash/parallel-xxhash.h"


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

class MyValue{
public:
    double accumulateOccupancy; // the past accumulated occupancy
    bool currentOccupancy; // occupancy of the current point cloud
    uint32_t currentPointCloud;
    MyValue(){}
    MyValue(double _accumulateOccupancy, bool _currentOccupancy, uint32_t _currentPointCloud){
        accumulateOccupancy = _accumulateOccupancy;
        currentOccupancy = _currentOccupancy;
        currentPointCloud = _currentPointCloud;
    }
    MyValue(const MyValue& m) {
        accumulateOccupancy = m.accumulateOccupancy;
        currentOccupancy = m.currentOccupancy;
        currentPointCloud = m.currentPointCloud;
    }
    void compress(double prob_miss_log, double prob_hit_log) {
        // merge current info into accumulate info
        if (currentOccupancy == false) {
            accumulateOccupancy += prob_miss_log;
        }
        else{
            accumulateOccupancy += prob_hit_log;
        }
    }
};


// Hash node class template
class HashNode {
public:
    HashNode(const OcTreeKey &_key, const MyValue& _myValue) {
        key = _key;
        myValue = _myValue;
    }

    HashNode(){}

    OcTreeKey getKey() const {
        return key;
    }

    MyValue getValue() const {
        return myValue;
    }

public:
    OcTreeKey key;
    MyValue myValue;
};

// Hash map class template
class HashMap {
public:

    HashMap(){
        itemCount = 0;
    }

    void init(uint32_t _TABLE_SIZE, OcTree* _tree) {
        // construct zero initialized hash table of size
        TABLE_SIZE = _TABLE_SIZE;
        table = new std::vector<HashNode>[TABLE_SIZE];
        clock = 0;
        currentPointCloud = 0;
        tree = _tree;
    }

    ~HashMap() {
        delete[] table;
    }

    float get(const OcTreeKey &key) {
        // this function waits to be finished
        // search each bucket after another, stops if found 10 consecutive full buckets
        unsigned long hashValue = MortonHash(key);
        for (auto it = table[hashValue].begin(); it != table[hashValue].end(); it++) {
            if (it->getKey() == key) {
                return it->getValue().accumulateOccupancy;
            }
        }
        // it is impossible to return a so large number as 100, note as not found
        return 100;
    }

    void KickToBuffer(ReaderWriterQueue<Item>* q, std::atomic_int& bufferSize);

    void put(const OcTreeKey &key, const bool &value, const uint32_t& hashValue);

    uint32_t ScalarHash(const OcTreeKey &key);

    uint32_t MortonHash(const OcTreeKey &key);

    // when the whole workflow ends, clean all the items that are stalk within the cache
    void cleanHashMap(ReaderWriterQueue<Item>* q, std::atomic_int& bufferSize);

    void KickToOctree();

public:
    // hash table
    std::vector<HashNode> *table;
    uint32_t TABLE_SIZE;
    uint32_t Columns = 4;
    uint32_t clock;
    uint32_t currentPointCloud; // # of current inserting point cloud
    uint32_t itemCount;
    OcTree* tree;
    uint32_t OcTreeKeyBuffer[3][8];
    OcTreeKeyValuePair BufferedPairs[8];
};

}

#endif