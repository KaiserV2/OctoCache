#ifndef PARAM_H
#define PARAM_H

#include <stdint.h>
#include <mutex>
#include <atomic>
#include <chrono>
#include <iostream>
#include "OcTreeKey.h"
#include <set>
#include <fstream>

#define USE_CACHE false
#define USE_NEW_CACHE true
#define VECTOR_OCTOMAP false // use vector instead of hash table to store voxels after ray tracing
#define USE_CQ false // true for using circular queue, false for using vector
#define FLOAT_TABLE true // true for using <OcTreeKey, float> for table, false for using <OcTreeKey, OcTreeNode*>
#define STATISTIC true

#define DEBUG_API true
#define ONE_THREAD false // the 1 threaded version of cache
#define DETAIL_LOG false
#define DEBUG1 false
#define DEBUG2 false
#define DETAIL_COUNT false

// #define _OPENMP
const int DEFAULT_TABLE_SIZE = 20;
#define SEED 19991228

// extern std::mutex mtx;
extern std::atomic_bool lock;


extern uint32_t fetch_from_octree;
// extern uint32_t insert_to_octree;
extern uint32_t insert_to_hashmap;
extern uint32_t insert_to_buffer;
extern uint32_t original_nodeupdate;
extern uint32_t free_nodeupdate;
extern uint32_t occupied_nodeupdate;


extern uint64_t hash_time;
extern uint64_t put_time;
extern uint64_t kick_time;
extern uint64_t insert_time;
extern uint64_t raytrace_time;
extern uint64_t duplicate_time;
extern uint64_t cachemiss_time;

extern uint64_t octree_time;
extern uint64_t begin_octree;
extern bool threadOn;
extern int countTotal;

extern std::fstream fout;

#define MAX_PRIME32 1229
#define MAX_BIG_PRIME32 50

extern uint32_t big_prime3232[MAX_BIG_PRIME32];
extern uint32_t prime32[MAX_PRIME32];


#define mix(a,b,c) \
{ \
  a -= b; a -= c; a ^= (c>>13); \
  b -= c; b -= a; b ^= (a<<8); \
  c -= a; c -= b; c ^= (b>>13); \
  a -= b; a -= c; a ^= (c>>12);  \
  b -= c; b -= a; b ^= (a<<16); \
  c -= a; c -= b; c ^= (b>>5); \
  a -= b; a -= c; a ^= (c>>3);  \
  b -= c; b -= a; b ^= (a<<10); \
  c -= a; c -= b; c ^= (b>>15); \
}


namespace octomap {
  // a set of OcTreeKey
  extern KeySet key_sets[3];
  extern int PCcount;
}

long long print_time(std::string s);

#endif