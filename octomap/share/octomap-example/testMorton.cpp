#include <fstream>
#include <iostream>
#include <string>
#include <octomap/OcTree.h>
#include <octomap/octomap_timing.h>
#include <random>
#include <unordered_set>

using namespace std;
using namespace octomap;

OcTreeKey keys[209517632];

OcTreeKey* synkeys;
uint32_t KeyCount = 0;
uint32_t CurrentPointCloud = 0;
uint32_t PrevPointCloud = -1;
ifstream fin;
bool value;
uint32_t totalOctreeInsertion = 0;
string datasetname;
uint32_t hashMapSize = 1;

vector<int> fvec;
vector<double> tvec;

const uint32_t TABLE_SIZE = 1 << 21;
uint32_t counters[TABLE_SIZE];

bool ReadKey(ifstream& fin){
    KeyCount = 0;
    string s;
    bool flag = false;
    while (getline(fin, s)){
        flag = true;
        if ((s == "next")) {
            break;
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
        KeyCount++;
    }
    return flag;
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

int main(int argc, char** argv) {
    fin.open("/proj/softmeasure-PG0/Peiqing/Dataset/Octomap/OctreeInsertion/2.txt");
    while(ReadKey(fin)){
        cout << KeyCount << endl;
        TestMorton();
    }
    return 0;
}