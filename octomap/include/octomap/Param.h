#ifndef PARAM_H
#define PARAM_H

#include <stdint.h>

#define USE_CACHE false
#define DETAIL_LOG false
#define DEBUG1 false
#define DEBUG2 false

#define _OPENMP
const int DEFAULT_TABLE_SIZE = 20;
#define SEED 19991228

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