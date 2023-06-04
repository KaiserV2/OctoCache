#ifndef PARAM_H
#define PARAM_H

#include <stdint.h>
#include <mutex>
#include <atomic>

#define USE_CACHE false
#define USE_NEW_CACHE false
#define ONE_THREAD false // the 1 threaded version of cache
#define DETAIL_LOG false
#define DEBUG1 false
#define DEBUG2 false
#define DETAIL_COUNT false

// #define _OPENMP
const int DEFAULT_TABLE_SIZE = 20;
#define SEED 19991228

extern std::mutex mtx;
extern std::atomic_bool lock;


extern uint32_t fetch_from_octree;
extern uint32_t insert_to_octree;
extern uint32_t insert_to_hashmap;
extern uint32_t insert_to_buffer;
extern uint32_t original_nodeupdate;


extern uint64_t hash_time;
extern uint64_t put_time;
extern uint64_t kick_time;
extern uint64_t insert_time;
extern uint64_t raytrace_time;

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


#endif