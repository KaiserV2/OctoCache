#include <fstream>
#include <iostream>
#include <string>
#include <octomap/OcTree.h>
#include <octomap/Cache.h>
#include <octomap/octomap_timing.h>
#include <random>
#include <unordered_set>
#include <thread>
#include <pthread.h>

using namespace std;
using namespace octomap;

OcTreeKey* keys;
bool* Occupancy;


uint32_t hashMapSize = 1 << 16;
uint32_t clockSpeed = 1;
uint32_t endPC = 0;



OcTreeKey* synkeys;
uint32_t KeyCount = 0;
uint32_t PCCount = 0;
uint32_t CurrentPointCloud = 0;
uint32_t PrevPointCloud = -1;
ifstream fin;
fstream fout;

bool value;
uint32_t totalOctreeInsertion = 0;
string datasetname;

vector<int> fvec;
vector<double> tvec;

const uint32_t TABLE_SIZE = 1 << 10;
uint32_t counters[TABLE_SIZE];

void SpecifyThread(){
    int s;
    pthread_t thread;
    cpu_set_t cpuset;
    thread = pthread_self();
    CPU_ZERO(&cpuset);
    CPU_SET(1, &cpuset);
    s = pthread_setaffinity_np(thread, sizeof(cpuset), &cpuset);
}

void ReadKey(ifstream& fin){
    string s;
    while (getline(fin, s)){
        if ((s == "next")) {
            PCCount++;
            continue;
            if (PCCount = endPC) {
                break;
            }
        }
        uint32_t pos0 = 0;
        uint32_t pos1 = s.find(" ", pos0 + 1);
        uint32_t pos2 = s.find(" ", pos1 + 1);
        uint32_t pos3 = s.find(" ", pos2 + 1);
        string line1 = s.substr(pos0, pos1);
        string line2 = s.substr(pos1 + 1 , pos2 - pos1 - 1);
        string line3 = s.substr(pos2 + 1, pos3 - pos2 - 1);
        uint16_t x;
        x = stoi(line1);
        keys[KeyCount].k[0] = x;
        x = stoi(line2);
        keys[KeyCount].k[1] = x;
        x = stoi(line3);
        keys[KeyCount].k[2] = x;
        string line4 = s.substr(s.length() - 1, s.length());
        Occupancy[KeyCount] = stoi(line4);
        KeyCount++;
    }
    return;
}

uint32_t MortonHash(const OcTreeKey &key){
    uint64_t sum = 0;
    std::bitset<16> k0(key.k[0]);
    std::bitset<16> k1(key.k[1]);
    std::bitset<16> k2(key.k[2]);
    for (int i = 15; i >= 0; i--) {
        sum *= 8;
        sum += k0.test(i) * 4 + k1.test(i) * 2 + k2.test(i);
    }
    uint32_t hashValue = sum % TABLE_SIZE;
    // uint32_t hashValue = hashFunc.run((const char *)(&(key.k[0])), sizeof(key_type) * 3) % TABLE_SIZE;
    return hashValue;
}

void TestMorton(){
    uint32_t max = 0;
    memset(counters, 0, sizeof(counters));
    cout << "point cloud size / table size is " << (double)KeyCount / (double)TABLE_SIZE << endl;
    for (int i = 0; i < KeyCount; i++) {
        uint32_t hashValue = MortonHash(keys[i]);
        counters[hashValue]++;
        if(counters[hashValue] > max) {
            max = counters[hashValue];
        }
    }
    cout << "the maximum hash collision is " << max << endl;
    cout << "the collision ratio is " << (double)max / ((double)KeyCount / (double)TABLE_SIZE) << endl;
}

int bin(double n){
    int ans = 1;
    while (n > 2) {
        n /= 2;
        ans++;
    }
    return ans;
}


int main(int argc, char** argv) {
    keys = new OcTreeKey[320000000];
    Occupancy = new bool[320000000];
    // SpecifyThread();
    int arg = 0;
    string graphFileNum = "1";
    string ss;
    string sk;
    while (++arg < argc) {
        if (! strcmp(argv[arg], "-i")){
            graphFileNum = std::string(argv[++arg]);
            if (graphFileNum == "1") {
                hashMapSize = 1 << 17;
                endPC = 66;
            }
            if (graphFileNum == "2") {
                hashMapSize = 1 << 22;
                endPC = 81;
            }
            if (graphFileNum == "3") {
                hashMapSize = 1 << 12;
                endPC = 92631;
            }
        }
        if (! strcmp(argv[arg], "-s")){
            ss = std::string(argv[++arg]);
            hashMapSize *= stoi(ss);
        }
        if (! strcmp(argv[arg], "-k")){
            sk = std::string(argv[++arg]);
            clockSpeed *= stoi(sk);
        }
    }

    fin.open("/home/peiqing/Dataset/Octomap/OctreeInsertion/" + graphFileNum + ".txt");
    ReadKey(fin);
    cout << "read in " << KeyCount << " OctreeKeys and " << PCCount << " point clouds" << endl;
    cout << "average Point cloud size 1<<" << bin((double)KeyCount / (double)(PCCount + 1)) << endl;
    OcTree * tree = new OcTree(0.1);

    string filename = "/proj/softmeasure-PG0/Peiqing/Dataset/Octomap/OctreeInsertion/" + datasetname + ".txt";
    fout.open("/home/peiqing/Dataset/Octomap/TableDistribution/" + graphFileNum + "-" + ss + "-" + sk + ".txt", std::fstream::app);
    

    Cache* myCache = new Cache(hashMapSize, tree, filename, clockSpeed);
    uint64_t st = _rdtsc();
    myCache->StartThread();
    for (int i = 0; i < KeyCount; i++) {
        myCache->ProcessPkt(keys[i], Occupancy[i]);
    }
    myCache->EndThread();
    uint64_t et = _rdtsc();
    cout << "thread 1 runtime " << et - st << endl;
    delete[] keys;
    delete[] Occupancy;
    cout << "insert_to_octree " << insert_to_octree << endl;
    cout << "fetch_from_octree " << fetch_from_octree << endl;

    return 0;

    int cacheItems = 0;
    for (int i = 0; i < myCache->myHashMap.TABLE_SIZE; i++) {
        fout << myCache->myHashMap.table[i].size() << ",";
        cacheItems += myCache->myHashMap.table[i].size();
    }
    fout << endl;
    fout << "cached items " << cacheItems << endl;
    fout << "fetch_from_octree " << fetch_from_octree << endl;
    fout << "insert_to_octree " << insert_to_octree << endl;
    fout << "insert_to_hashmap " << insert_to_hashmap << endl;
    fout << "insert_to_buffer " << insert_to_buffer << endl;
    // cout << "hash time " << hash_time << endl;
    // cout << "put time " << put_time << endl;
    // cout << "kick time " << kick_time << endl;
    
}

