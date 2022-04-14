#ifndef CACHE
#define CACHE

#include "OcTreeKey.h"
#include "HashMap.h"
#include <iostream>
#include <queue>
#include "GlobalVariables_Octomap.h"
#include "OcTreeNode.h"

namespace octomap{


class OcTree; // every function concerned with OcTree only passes OcTree as a pointer



class Cache{
public:
    // have a buffer
    queue<Item> buffer;
    HashMap myHashMap;
    OcTree* tree;
    // function to feed items in the buffer to the octree

    Cache() {}
    ~Cache() {}

    void DigestBuffer();
    void PrintBuffer();
    void test();
    // void test(AbstractOcTree* tree);

};

}
#endif

