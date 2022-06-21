#ifndef HASH_MAP_H
#define HASH_MAP_H

#include "Param.h"
#include "BOBHash32.h"
#include <queue>
#include <atomic>
#include <iostream>
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
        // only happens when a new node is created
        next = NULL;
        key = _key;
        myValue = _myValue;
    }

    OcTreeKey getKey() const {
        return key;
    }

    MyValue getValue() const {
        return myValue;
    }

    HashNode *getNext() const {
        return next;
    }

    void setNext(HashNode *next) {
        this->next = next;
    }

public:
    // key-value pair
    OcTreeKey key;
    MyValue myValue;
    // next bucket with the same key
    HashNode *next;
};

// Hash map class template
class HashMap {
public:

    HashMap(){
        OcTreeKeyBufferSize = 0;
        hashSeed = 0x3afc8e77;
        itemCount = 0;
    }

    void init(uint32_t _TABLE_SIZE, OcTree* _tree) {
        // construct zero initialized hash table of size
        TABLE_SIZE = _TABLE_SIZE;
        table = new HashNode *[TABLE_SIZE]();
#if DEBUG1
        printf("HashMap created!\n");
        // printf("Initialized with seed %lu", &x);
#endif
        hashFunc.initialize(122);
        clock = 0;
        currentPointCloud = 0;
        tree = _tree;
    }

    unsigned long MyKeyHash(const OcTreeKey& key)
    {
// #if DETAIL_LOG
//         printf("get to MyKeyHash\n");
//         printf("%lu\n",sizeof(key_type) * 3);
//         std::cout << key.k << std::endl;
//         std::cout << &(key.k[0]) << std::endl;
//         std::cout << &(key.k[1]) << std::endl;
//         std::cout << &(key.k[2]) << std::endl;
// #endif
        uint32_t tmp = hashFunc.run((const char *)(&(key.k[0])), sizeof(key_type) * 3);
        uint32_t ans = tmp % TABLE_SIZE;
#if DETAIL_LOG
        printf("hash value %d\n", ans);
#endif
        return ans;
    }

    ~HashMap() {
        // destroy all buckets one by one
        for (uint32_t i = 0; i < TABLE_SIZE; ++i) {
            HashNode *entry = table[i];
            while (entry != NULL) {
                HashNode *prev = entry;
                entry = entry->getNext();
                delete prev;
            }
            table[i] = NULL;
        }
        // destroy the hash table
        delete [] table;
    }

    bool get(const OcTreeKey &key, MyValue &value) {
        unsigned long hashValue = MyKeyHash(key);
        HashNode *entry = table[hashValue];

        while (entry != NULL) {
            if (entry->getKey() == key) {
                value = entry->getValue();
                return true;
            }
            entry = entry->getNext();
        }
        return false;
    }

    void test() {
        printf("The HashMap has size %d", TABLE_SIZE);
    }

    void KickToBuffer(ReaderWriterQueue<Item>* q, std::atomic_int& bufferSize);

    // void put(const OcTreeKey &key, const bool &value);
    void put(const OcTreeKey &key, const bool &value, const uint32_t& hashValue);

    void store(const OcTreeKey &key, const bool &value);

    void flush();

    uint32_t ScalarHash(const OcTreeKey &key, const bool &value);

    // when the whole workflow ends, clean all the items that are stalk within the cache
    void cleanHashMap(ReaderWriterQueue<Item>* q, std::atomic_int& bufferSize);

    void remove(const OcTreeKey &key) {
        unsigned long hashValue = MyKeyHash(key);
        HashNode *prev = NULL;
        HashNode *entry = table[hashValue];

        while (entry != NULL && entry->getKey() != key) {
            prev = entry;
            entry = entry->getNext();
        }

        if (entry == NULL) {
            // key not found
            return;
        }
        else {
            if (prev == NULL) {
                // remove first bucket of the list
                table[hashValue] = entry->getNext();
            } else {
                prev->setNext(entry->getNext());
            }
            delete entry;
        }
    }

    void KickToOctree();

public:
    // hash table
    HashNode **table;
    uint32_t TABLE_SIZE;
    BOBHash32 hashFunc;
    uint32_t clock;
    uint32_t currentPointCloud; // # of current inserting point cloud
    uint32_t itemCount;
    OcTree* tree;
    uint32_t hashSeed;
    uint32_t OcTreeKeyBuffer[3][8];
    uint32_t OcTreeKeyBufferSize;
    OcTreeKeyValuePair BufferedPairs[8];
};

}

#endif