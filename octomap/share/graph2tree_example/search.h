#ifndef CAST_RAY_H
#define CAST_RAY_H

#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

#include <octomap/octomap.h>
#include <octomap/octomap_timing.h>
#include <octomap/GlobalVariables_Cache.h>
#include <octomap/Cache.h>
#include <octomap/Param.h>
#include <omp.h>
#include <sys/time.h>

using namespace std;
using namespace octomap;

class MyInt {
public:
    int value;
    MyInt() {}
    MyInt(int v) {
        value = v;
    }
    int getValue() {
        return value;
    }
};

void test(){
  ReaderWriterQueue<int> q;
  MyInt* a = new MyInt(1);
  q.enqueue(a->getValue());
  delete a;
  int b;
  q.try_dequeue(b);
  cout << b << endl;
}

void testSearch(Cache* myCache, OcTree* tree){
  cout << "OctoCache:" << endl;
  cout << tree->getProbHitLog() << " " << tree->getProbMissLog() << endl;
  cout << tree->getClampingThresMinLog() << " " << tree->getClampingThresMaxLog() << endl;
  const uint16_t keys[15][3] = {
        {32897, 32762, 32767},
        {32903, 32762, 32767},
        {32904, 32762, 32767},
        {32905, 32762, 32767},
        {32906, 32762, 32767},
        {32915, 32762, 32767},
        {32916, 32762, 32767},
        {32917, 32762, 32767},
        {32918, 32762, 32767},
        {32919, 32762, 32767},
        {32920, 32762, 32767},
        {32784, 32773, 32777},
        {32921, 32762, 32767},
        {32785, 32773, 32777},
        {32789, 32773, 32777}
    };
    for (int i = 0; i < 15; i++) {
        // form OcTreeKeys
        OcTreeKey key(keys[i][0],keys[i][1],keys[i][2]);
        // float ans = myCache->search(key);
        point3d point = tree->keyToCoord(key);
        auto node = myCache->search(point(0), point(1), point(2));
        if (node == NULL) {
          cout << "error " << endl;
        }
        else {
          cout << node->getOccupancy() << endl;
        }
    }
}

void testSearch(OcTree* tree){
  cout << "Baseline:" << endl;
  cout << tree->getClampingThresMinLog() << " " << tree->getClampingThresMaxLog() << endl;
  const uint16_t keys[15][3] = {
        {32897, 32762, 32767},
        {32903, 32762, 32767},
        {32904, 32762, 32767},
        {32905, 32762, 32767},
        {32906, 32762, 32767},
        {32915, 32762, 32767},
        {32916, 32762, 32767},
        {32917, 32762, 32767},
        {32918, 32762, 32767},
        {32919, 32762, 32767},
        {32920, 32762, 32767},
        {32784, 32773, 32777},
        {32921, 32762, 32767},
        {32785, 32773, 32777},
        {32789, 32773, 32777}
  };
  for (int i = 0; i < 15; i++) {
    // form OcTreeKeys
    OcTreeKey key(keys[i][0],keys[i][1],keys[i][2]);
    // auto ans = tree->search(key);
    point3d point = tree->keyToCoord(key);
    auto node = tree->search(point(0), point(1), point(2));
    if (node == NULL) {
      cout << "error " << endl;
    }
    else {
      cout << node->getOccupancy() << endl;
    }
  }
}


#endif