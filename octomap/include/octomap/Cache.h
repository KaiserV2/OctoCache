#ifndef CACHE
#define CACHE

#include "OcTreeKey.h"
#include "HashMap.h"
#include <iostream>
#include <queue>
#include "GlobalVariables_Octomap.h"

namespace octomap{

class Item{
public:
    OcTreeKey key;
    MyQueue myQueue;
    Item(){}
    Item(Item & itm){
        key = itm.key;
        myQueue = itm.myQueue;
    }
    ~Item(){}
};

class Cache{
public:
    // have a buffer
    queue<Item> buffer;
    // function to feed items in the buffer to the octree

    Cache() {}
    ~Cache() {}

    void DigestBuffer() {
        tree->test();
        while (buffer.size() > 0) {
            Item item = buffer.front();
            buffer.pop();
            OcTreeKey key = item.key;
            MyQueue myQueue = item.myQueue;
            // digest a single item, containing several updates of one voxel to the octree
            while (myQueue.dq.size() > 0) {
                bool occupancy = myQueue.dq.front().occupancyCount;
                if (occupancy == true) {
                    // its an occupied voxel
                    tree->updateNode(key, true, lazy_eval);
                }
                else {
                    // a free voxel
                    tree->updateNode(key, false, lazy_eval);
                }
            }
        }
    }
};

}
#endif

