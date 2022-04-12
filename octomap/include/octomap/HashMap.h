#ifndef HASH_MAP_H
#define HASH_MAP_H

#include "Param.h"
#include "BOBHash32.h"
#include <queue>
#include <iostream>
#include <string.h>
#include "OcTreeKey.h"
#include "GlobalVariables_Octomap.h"

namespace octomap{

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
    deque<MyPair> dq;
    MyQueue(){}
    MyQueue(MyQueue& mq){
        this->dq = mq.dq;
    }
    void Update(int _pcNum, bool val) {
#ifdef DETAIL_LOG
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

// Hash node class template
class HashNode {
public:
    HashNode(const OcTreeKey &key, const bool &value) {
        // only happens when a new node is created
        next = NULL;
        this->key = key;
        MyPair tmp = MyPair(pointCloudCount, value);
        this->value.dq.push_back(tmp);
    }

    OcTreeKey getKey() const {
        return key;
    }

    MyQueue getValue() {
        return value;
    }

    void setValue(bool value) {
        this->value.Update(pointCloudCount, value);
    }

    HashNode *getNext() const {
        return next;
    }

    void setNext(HashNode *next) {
        this->next = next;
    }

private:
    // key-value pair
    OcTreeKey key;
    MyQueue value;
    // next bucket with the same key
    HashNode *next;
};

// Hash map class template
class HashMap {
public:
    HashMap() {
        // construct zero initialized hash table of size
        table = new HashNode *[TABLE_SIZE]();
        // printf("HashMap created!\n");
        // initialize the BOBHash32
        hashFunc.initialize(SEED % MAX_PRIME32);
    }

    unsigned long MyKeyHash(const OcTreeKey& key)
    {
#ifdef DETAIL_LOG
        printf("get to MyKeyHash\n");
        printf("%lu\n",sizeof(key_type) * 3);
        std::cout << key.k << std::endl;
        std::cout << &(key.k[0]) << std::endl;
        std::cout << &(key.k[1]) << std::endl;
        std::cout << &(key.k[2]) << std::endl;
#endif
        uint32_t tmp = hashFunc.run((const char *)(&(key.k[0])), sizeof(key_type) * 3);
        uint32_t ans = tmp % TABLE_SIZE;
#ifdef DETAIL_LOG
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

    bool get(const OcTreeKey &key, MyQueue &value) {
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

    void put(const OcTreeKey &key, const bool &value) {
#ifdef DETAIL_LOG
        printf("get to put\n");
#endif

        unsigned long hashValue = MyKeyHash(key);
        // printf("%lu", hashValue);
        HashNode *prev = NULL;
        HashNode *entry = table[hashValue];

        while (entry != NULL && entry->getKey() != key) {
            prev = entry;
            entry = entry->getNext();
        }

        if (entry == NULL) {
            entry = new HashNode(key, value);
            if (prev == NULL) {
                // insert as first bucket
                table[hashValue] = entry;
            } else {
                prev->setNext(entry);
            }
        } else {
            // just update the value
            entry->setValue(value); // change here
        }
    }

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

private:
    // hash table
    HashNode **table;
    BOBHash32 hashFunc;
};

}

#endif