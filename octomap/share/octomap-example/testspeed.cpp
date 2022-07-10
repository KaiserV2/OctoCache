#include <fstream>
#include <iostream>
#include <string>
#include <octomap/OcTree.h>
#include <octomap/octomap_timing.h>
#include <random>

using namespace std;
using namespace octomap;

OcTreeKey keys[209517632];
uint32_t KeyCount = 0;
uint32_t CurrentPointCloud = 0;
uint32_t PrevPointCloud = -1;
ifstream fin;
bool value;
uint32_t totalOctreeInsertion = 0;
string datasetname;
uint32_t hashMapSize = 1;

bool compareXYZ(OcTreeKey a, OcTreeKey b){
    if (a.k[0] < b.k[0]){
        return true;
    }
    if (a.k[0] > b.k[0]){
        return false;
    }
    if (a.k[1] < b.k[1]){
        return true;
    }
    if (a.k[1] > b.k[1]){
        return false;
    }
    if (a.k[2] < b.k[2]){
        return true;
    }
    if (a.k[2] > b.k[2]){
        return false;
    }
    return false;
}

bool compareYZX(OcTreeKey a, OcTreeKey b){
    if (a.k[1] < b.k[1]){
        return true;
    }
    if (a.k[1] > b.k[1]){
        return false;
    }
    if (a.k[2] < b.k[2]){
        return true;
    }
    if (a.k[2] > b.k[2]){
        return false;
    }
    if (a.k[0] < b.k[0]){
        return true;
    }
    if (a.k[0] > b.k[0]){
        return false;
    }
    return false;
}

bool compareZXY(OcTreeKey a, OcTreeKey b){
    if (a.k[2] < b.k[2]){
        return true;
    }
    if (a.k[2] > b.k[2]){
        return false;
    }
    if (a.k[0] < b.k[0]){
        return true;
    }
    if (a.k[0] > b.k[0]){
        return false;
    }
    if (a.k[1] < b.k[1]){
        return true;
    }
    if (a.k[1] > b.k[1]){
        return false;
    }
    return false;
}

bool compareCube(OcTreeKey a, OcTreeKey b){
    for (int i = 0; i < 16; i++) {
        if ((a.k[0] >> (15 - i)) <  (b.k[0] >> (15 - i))){
            return true;
        }
        if ((a.k[0] >> (15 - i)) >  (b.k[0] >> (15 - i))){
            return false;
        }
        if ((a.k[1] >> (15 - i)) <  (b.k[1] >> (15 - i))){
            return true;
        }
        if ((a.k[1] >> (15 - i)) >  (b.k[1] >> (15 - i))){
            return false;
        }
        if ((a.k[2] >> (15 - i)) <  (b.k[2] >> (15 - i))){
            return true;
        }
        if ((a.k[2] >> (15 - i)) >  (b.k[2] >> (15 - i))){
            return false;
        }
    }
    return false;
}

void Insert(){
    OcTree* tree = new OcTree(0.1);
    for (int i = 0; i < KeyCount; i++) {
        tree->updateNode(keys[i], false, false);
    }
}

void Original(){
    timeval start;
    timeval stop;
    gettimeofday(&start, NULL);
    Insert();
    gettimeofday(&stop, NULL);
    double time_to_insert = (stop.tv_sec - start.tv_sec) + 1.0e-6 *(stop.tv_usec - start.tv_usec);
    cout << "The original order takes " << time_to_insert << " seconds to insert to Octree" << endl;
}

void RandomShuffle(){
    srand((unsigned)time(NULL));
    std::random_shuffle(&keys[0], &keys[KeyCount]);
    timeval start;
    timeval stop;
    gettimeofday(&start, NULL);
    Insert();
    gettimeofday(&stop, NULL);
    double time_to_insert = (stop.tv_sec - start.tv_sec) + 1.0e-6 *(stop.tv_usec - start.tv_usec);
    cout << "The randomly shuffled order takes " << time_to_insert << " seconds to insert to Octree" << endl;
}

void XYZSort(){
    sort(&keys[0], &keys[KeyCount], compareXYZ);
    timeval start;
    timeval stop;
    gettimeofday(&start, NULL);
    Insert();
    gettimeofday(&stop, NULL);
    double time_to_insert = (stop.tv_sec - start.tv_sec) + 1.0e-6 *(stop.tv_usec - start.tv_usec);
    cout << "The sort XYZ order takes " << time_to_insert << " seconds to insert to Octree" << endl;
}

void YZXSort(){
    sort(&keys[0], &keys[KeyCount], compareYZX);
    timeval start;
    timeval stop;
    gettimeofday(&start, NULL);
    Insert();
    gettimeofday(&stop, NULL);
    double time_to_insert = (stop.tv_sec - start.tv_sec) + 1.0e-6 *(stop.tv_usec - start.tv_usec);
    cout << "The sort YZX order takes " << time_to_insert << " seconds to insert to Octree" << endl;
}

void ZXYSort(){
    sort(&keys[0], &keys[KeyCount], compareZXY);
    timeval start;
    timeval stop;
    gettimeofday(&start, NULL);
    Insert();
    gettimeofday(&stop, NULL);
    double time_to_insert = (stop.tv_sec - start.tv_sec) + 1.0e-6 *(stop.tv_usec - start.tv_usec);
    cout << "The sort ZXY order takes " << time_to_insert << " seconds to insert to Octree" << endl;
}

void CubeSort(){
    sort(&keys[0], &keys[KeyCount], compareCube);
    timeval start;
    timeval stop;
    gettimeofday(&start, NULL);
    Insert();
    gettimeofday(&stop, NULL);
    double time_to_insert = (stop.tv_sec - start.tv_sec) + 1.0e-6 *(stop.tv_usec - start.tv_usec);
    cout << "The sort Cube order takes " << time_to_insert << " seconds to insert to Octree" << endl;
}

void ReadKey(ifstream& fin){
    string s;
    while (getline(fin, s)){
        if (s == "next") {
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
    return;
}

int main(int argc, char** argv) {
    int arg = 0;
    string graphFileNum;
    string mapSizeNum;
    while (++arg < argc) {
        if (! strcmp(argv[arg], "-i")){
        graphFileNum = std::string(argv[++arg]);
        if (graphFileNum == "1"){
            datasetname = "fr_079";
        }
        else if (graphFileNum == "2"){
            datasetname = "fr_campus";
        }
        else if (graphFileNum == "3"){
            datasetname = "new_college";
        }
        else{ // default
            datasetname = "fr_079";
        }
        }
        else if (!strcmp(argv[arg], "-s")){ // specify the size of the hash table
            mapSizeNum = std::string(argv[++arg]);
            int tmp = stoi(mapSizeNum);
            for (int i = 0; i < tmp; i++){
                hashMapSize *= 10;
            }
        }
    }
    fin.open("/proj/softmeasure-PG0/Peiqing/Dataset/Octomap/OctreeInsertion/1.txt");
    ReadKey(fin);
    Original();
    cout << keys[10000].k[0] << endl;
    RandomShuffle();
    cout << keys[10000].k[0] << endl;
    XYZSort();
    cout << keys[10000].k[0] << endl;
    YZXSort();
    cout << keys[10000].k[0] << endl;
    ZXYSort();
    cout << keys[10000].k[0] << endl;
    CubeSort();
    cout << keys[10000].k[0] << endl;
    cout << "processed " << KeyCount << " keys of " << CurrentPointCloud << " point clouds" << endl; 
    // cout << "updated "<< totalOctreeInsertion << " nodes to Octree" << endl;
    return 0;
}