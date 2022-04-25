#ifndef HASH_MAP_H
#define HASH_MAP_H

#include "Param.h"
#include "BOBHash32.h"
#include <queue>
#include <iostream>
#include <string.h>
#include "OcTreeKey.h"
#include "multi-core/readerwriterqueue.h"

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
    bool occupancy;
    Item(){}
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
};


// Hash node class template
class HashNode {
public:
    HashNode(const OcTreeKey &key, const bool &value) {
        // only happens when a new node is created
        next = NULL;
        this->key = key;
        MyPair tmp = MyPair(pointCloudCount, value);
        this->myValue.dq.push_back(tmp);
    }

    OcTreeKey getKey() const {
        return key;
    }

    MyQueue getValue() const {
        return myValue;
    }

    void setValue(bool value) {
        this->myValue.Update(pointCloudCount, value);
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

    HashMap(){}

    void init(uint32_t _TABLE_SIZE, uint32_t _clockWait, OcTree* _tree) {
        // construct zero initialized hash table of size
        TABLE_SIZE = _TABLE_SIZE;
        clockWait = _clockWait;
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
        for (int i = 0; i < TABLE_SIZE; ++i) {
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

    // kick key and form item, then put to buffer
    void KickToBuffer(ReaderWriterQueue<Item>* q, atomic_int* bufferSize){
    // void KickToBuffer(std::queue<Item>* q){
#if DEBUG1
        std::cout << "Kicking position " << clock << std::endl;
#endif
        // remove all KV at position clock
        while(table[clock] != NULL) {
            HashNode* entry = table[clock];
            // we find a KV 
            OcTreeKey key = entry->getKey();
            MyQueue tmpQueue = entry->getValue();
            while(tmpQueue.dq.size() != 0) {
                Item item;
                item.key = key;
                item.occupancy = tmpQueue.dq.front().occupancyCount;
#if DEBUG1
        std::cout << "Putting to buffer: ";
        item.PrintItem();
#endif
                // q->push(item);
                q->enqueue(item);
                tmpQueue.dq.pop_front();
            }
            table[clock] = entry->getNext();
            delete entry;
        }
        clock++;
        if (clock == TABLE_SIZE) {
            clock = 0;
        }
        *bufferSize++;
    }

    // when the whole workflow ends, clean all the items that are stalk within the cache
    void cleanHashMap(ReaderWriterQueue<Item>* q) {
        printf("Cleaning the HashMap\n");
        for (uint32_t i = 0; i < TABLE_SIZE; i++) {
            KickToBuffer(q);
        }
    }

    void put(const OcTreeKey &key, const bool &value) {};

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

public:
    // hash table
    HashNode **table;
    uint32_t TABLE_SIZE;
    BOBHash32 hashFunc;
    uint32_t clock;
    uint32_t currentPointCloud; // # of current inserting point cloud
    OcTree* tree;
};

}

#endif