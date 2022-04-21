#ifndef ABSTRACT_H
#define ABSTRACT_H

#include "hash.h"

#include "Heap.h"
#include "readerwriterqueue.h"

#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>
#include <vector>

#include <cstring>
#include <pthread.h>

#include <xmmintrin.h>

#define HASH_NUM 3
#define LENGTH (1 << 16)
#define HEAP_SIZE 0x3ff

#define THP_TIME 200

#define CDF_NUMBER 1500

//#define RR

#define INTERVAL 20000
#define ALPHA 0.0002

//#define ACCURACY
//#define CORECYCLE
//#define THROUGHPUT
//#define CYCLEDIS
#define LATENCY
//#define MERGEDELAY
//#define QUEUELENGTH

#ifdef RR
    #define Partition RRPartition
#else
    #define Partition HashPartition
#endif

#ifdef ACCURACY
    #define SIGNAL std::atomic_int
#else
    #define SIGNAL int32_t
#endif

static std::atomic_int32_t PROMASK(0x7f);
static std::mutex mtx;

class Abstract{
public:
    typedef std::unordered_map<uint64_t, int32_t> HashMap;

    virtual void update(void* start, uint32_t size, HashMap mp) = 0;
    virtual ~Abstract(){};

#ifdef __linux__
    static bool setaffinity(std::thread* thd, uint32_t coreId){
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(coreId, &cpuset);
        int rc = pthread_setaffinity_np(thd->native_handle(),
                                        sizeof(cpu_set_t), &cpuset);
        if (rc != 0) {
            std::cerr << "Error calling pthread_setaffinity_np: " << rc << "\n";
            return false;
        }
        return true;
    }
#endif

    template<typename Key, uint32_t thread_num>
    inline static void RRPartition(Key* start, uint32_t size, uint32_t id, std::vector<Key>& vec){
        for(uint32_t i = id;i < size;i += thread_num){
            vec.push_back(start[i]);
        }
    }

    template<typename Key, uint32_t thread_num>
    inline static void HashPartition(Key* start, uint32_t size, uint32_t id, std::vector<Key>& vec){
        for(uint32_t i = 0;i < size;++i){
            if(hash(start[i], 101) % thread_num == id){
                vec.push_back(start[i]);
            }
        }
    }

    inline static void CDF(uint32_t* array, uint32_t length){
        double sum = 0, preSum = 0;

        for(uint32_t i = 0;i < length;++i){
            sum += array[i];
        }

        mtx.lock();
        for(uint32_t i = 0;i < length;++i){
            preSum += array[i];
            std::cout << preSum / sum << ",";
        }

        std::cout << std::endl;
        std::cout << std::endl;
        mtx.unlock();
    }

    inline static void Rank(double* array, uint32_t length){
        std::sort(array, array + length);

        /*
        std::cout << "MEDIAN: " << array[uint32_t(0.5 * length)] << std::endl;
        std::cout << "90%: " << array[uint32_t(0.9 * length)] << std::endl;
        std::cout << "95%: " << array[uint32_t(0.95 * length)] << std::endl;
        std::cout << "99%: " << array[uint32_t(0.99 * length)] << std::endl;
        std::cout << "99.5%: " << array[uint32_t(0.995 * length)] << std::endl;
        std::cout << "99.9%: " << array[uint32_t(0.999 * length)] << std::endl;
        */

        for(uint32_t i = 1;i < 1000;++i){
            std::cout << array[uint32_t(0.001 * i * length)] << ",";
        }

        std::cout << std::endl;

        std::cout << std::endl;
    }

    static void HHCompare(HashMap test, HashMap real, int32_t threshold){
        double estHH = 0, HH = 0, both = 0;
        double CR = 0, PR = 0, AAE = 0, ARE = 0;

        for(auto it = test.begin();it != test.end();++it){
            if(it->second > threshold){
                estHH += 1;
                if(real[it->first] > threshold){
                    both += 1;
                    AAE += abs(real[it->first] - it->second);
                    ARE += abs(real[it->first] - it->second) / (double)real[it->first];
                }
            }
        }

        for(auto it = real.begin();it != real.end();++it){
            if(it->second > threshold){
                HH += 1;
            }
        }

        //std::cout << estHH << " " << HH << " " << threshold <<  std::endl;

        std::cout << "CR: " << both / HH << std::endl
                  << "PR: " << both / estHH << std::endl
                  << "AAE: " << AAE / both << std::endl
                  << "ARE: " << ARE / both << std::endl << std::endl;
    }
};

#endif
