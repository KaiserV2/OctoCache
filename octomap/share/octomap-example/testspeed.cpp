#include <fstream>
#include <iostream>
#include <string>
#include <octomap/OcTree.h>
#include <octomap/octomap_timing.h>
#include <random>

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

void PrintResult(){
    for(auto it = fvec.begin(); it != fvec.end(); it++){
        cout << *it << ",";    
    }
    cout << endl;
    for(auto it = tvec.begin(); it != tvec.end(); it++){
        cout << *it << ",";    
    }
    cout << endl;
}

uint32_t distance(OcTreeKey& a, OcTreeKey& b){
    // calculate distance between 2 voxels based on tree distance
    // cout << bitset<sizeof(a.k[0])*8>(a.k[0]) << " " << bitset<sizeof(a.k[1])*8>(a.k[1]) << " " << bitset<sizeof(a.k[2])*8>(a.k[2]) << endl;
    // cout << bitset<sizeof(b.k[0])*8>(b.k[0]) << " " << bitset<sizeof(b.k[1])*8>(b.k[1]) << " " << bitset<sizeof(b.k[2])*8>(b.k[2]) << endl;
    uint32_t dis = 16;
    for (int i = 0; i < 16; i++) {
        if((a.k[0] >> (15 - i)) == (b.k[0] >> (15 - i)) && 
        (a.k[1] >> (15 - i)) == (b.k[1] >> (15 - i)) &&
        (a.k[2] >> (15 - i)) == (b.k[2] >> (15 - i))){
            dis--;
        }
        else{
            break;
        }
    }
    return dis;
}

uint32_t function(OcTreeKey* keys, uint32_t len){
    // calculate the function value based on a key array and length
    uint32_t sum = 0;
    for(int i = 0; i < len - 1; i++) {
        sum += distance(keys[i], keys[i + 1]);
    }
    return sum;
}

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
    uint32_t dis = function(keys, KeyCount);
    timeval start;
    timeval stop;
    gettimeofday(&start, NULL);
    Insert();
    gettimeofday(&stop, NULL);
    double time_to_insert = (stop.tv_sec - start.tv_sec) + 1.0e-6 *(stop.tv_usec - start.tv_usec);
    cout << "The original order: f = " << dis << " t = " << time_to_insert << endl;
    fvec.push_back(dis);
    tvec.push_back(time_to_insert);
}

void RandomShuffle(){
    srand((unsigned)time(NULL));
    std::random_shuffle(&keys[0], &keys[KeyCount]);
    uint32_t dis = function(keys, KeyCount);
    timeval start;
    timeval stop;
    gettimeofday(&start, NULL);
    Insert();
    gettimeofday(&stop, NULL);
    double time_to_insert = (stop.tv_sec - start.tv_sec) + 1.0e-6 *(stop.tv_usec - start.tv_usec);
    // cout << "The randomly shuffled order: f = " << dis << " t = " << time_to_insert << endl;
    fvec.push_back(dis);
    tvec.push_back(time_to_insert);
}

void XYZSort(){
    sort(&keys[0], &keys[KeyCount], compareXYZ);
    uint32_t dis = function(keys, KeyCount);
    timeval start;
    timeval stop;
    gettimeofday(&start, NULL);
    Insert();
    gettimeofday(&stop, NULL);
    double time_to_insert = (stop.tv_sec - start.tv_sec) + 1.0e-6 *(stop.tv_usec - start.tv_usec);
    cout << "The sort XYZ order takes: f = " << dis << " t = " << time_to_insert << endl;
    fvec.push_back(dis);
    tvec.push_back(time_to_insert);
}

void YZXSort(){
    sort(&keys[0], &keys[KeyCount], compareYZX);
    uint32_t dis = function(keys, KeyCount);
    timeval start;
    timeval stop;
    gettimeofday(&start, NULL);
    Insert();
    gettimeofday(&stop, NULL);
    double time_to_insert = (stop.tv_sec - start.tv_sec) + 1.0e-6 *(stop.tv_usec - start.tv_usec);
    cout << "The sort YZX order takes: f = " << dis << " t = " << time_to_insert << endl;
    fvec.push_back(dis);
    tvec.push_back(time_to_insert);
}

void ZXYSort(){
    sort(&keys[0], &keys[KeyCount], compareZXY);
    uint32_t dis = function(keys, KeyCount);
    timeval start;
    timeval stop;
    gettimeofday(&start, NULL);
    Insert();
    gettimeofday(&stop, NULL);
    double time_to_insert = (stop.tv_sec - start.tv_sec) + 1.0e-6 *(stop.tv_usec - start.tv_usec);
    cout << "The sort ZXY order takes: f = " << dis << " t = " << time_to_insert << endl;
    fvec.push_back(dis);
    tvec.push_back(time_to_insert);
}

void CubeSort(){
    sort(&keys[0], &keys[KeyCount], compareCube);
    uint32_t dis = function(keys, KeyCount);
    timeval start;
    timeval stop;
    gettimeofday(&start, NULL);
    Insert();
    gettimeofday(&stop, NULL);
    double time_to_insert = (stop.tv_sec - start.tv_sec) + 1.0e-6 *(stop.tv_usec - start.tv_usec);
    cout << "The sort Cube order takes: f = " << dis << " t = " << time_to_insert << endl;
    fvec.push_back(dis);
    tvec.push_back(time_to_insert);
}

int pos[9];

void FindSubCube(){
    int value = 0;
    pos[value] = 0;
    pos[8] = KeyCount + 1;
    for (int i = 0; i < KeyCount; i++) {
        if((keys[i].k[0]>>15)*4+(keys[i].k[1]>>15)*2+(keys[i].k[2]>>15) != value) {
            value++;
            pos[value] = i;
        }
    }
    for (int i = 0; i < 8; i++) {
        cout << pos[i] << " ";
    }
    cout << endl;
}

void SubCubeShuffle(){
    srand((unsigned)time(NULL));
    for (int i = 0; i < 8; i++) {
        std::random_shuffle(&keys[pos[i]], &keys[pos[i+1]]);
    }
    // std::random_shuffle(&keys[0], &keys[KeyCount]);
    uint32_t dis = function(keys, KeyCount);
    timeval start;
    timeval stop;
    gettimeofday(&start, NULL);
    Insert();
    gettimeofday(&stop, NULL);
    double time_to_insert = (stop.tv_sec - start.tv_sec) + 1.0e-6 *(stop.tv_usec - start.tv_usec);
    // cout << "The randomly shuffled order: f = " << dis << " t = " << time_to_insert << endl;
    fvec.push_back(dis);
    tvec.push_back(time_to_insert);
}

void DisStat(){
    // how far is a specific voxel from all other voxels
    int pick = rand() % KeyCount;
    int stat[17] = {0};
    for (int i = 0; i < KeyCount; i++) {
        stat[distance(keys[pick], keys[i])]++;
    }
    cout << "printing distance stat" << endl;
    for (int i = 0; i < 17; i++) {
        cout << stat[i] << ",";
    }
    cout << endl;
}

void ReadKey(ifstream& fin){
    string s;
    bool flag = false;
    while (getline(fin, s)){
        // if ((s == "next")) {
        //     break;
        // }
        if ((s == "next") && (flag == false)) {
            flag = true;
            continue;
        }
        if ((s == "next") && (flag == true)) {
            break;
        }
        if (flag == false){
            continue;
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

void SyntheticTrace(){
    KeyCount = 0;
    int sideLength = 512;
    synkeys = new OcTreeKey[sideLength * sideLength * sideLength];
    for (int x = 0; x < sideLength; x++) {
        for (int y = 0; y < sideLength; y++) {
            for (int z = 0; z < sideLength; z++) {
                synkeys[KeyCount].k[0] = x;
                synkeys[KeyCount].k[0] = y;
                synkeys[KeyCount].k[0] = z;
                KeyCount++;
            }
        }
    }
    sort(&synkeys[0], &synkeys[KeyCount], compareCube);
}

void print(){
    for (int i = 0; i < 10; i++) {
        cout << keys[i].k[0] << " " << keys[i].k[1] << endl;
    }
}

void Merge(int k){
    int count = 0;
    for (int j = 0; j < KeyCount / k; j++) {
        for (int i = 0; i < k; i++){
            keys[count] = synkeys[i * KeyCount / k + j];
            count++;
        }
    }
    timeval start;
    timeval stop;
    gettimeofday(&start, NULL);
    Insert();
    gettimeofday(&stop, NULL);
    double time_to_insert = (stop.tv_sec - start.tv_sec) + 1.0e-6 *(stop.tv_usec - start.tv_usec);
    cout << "The merge " << k << " order takes " << time_to_insert << " seconds to insert to Octree" << endl;
}

void DoMorton(){

}

void DoHash(){

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
    fin.open("/proj/softmeasure-PG0/Peiqing/Dataset/Octomap/OctreeInsertion/2.txt");
    ReadKey(fin);
    // cout << function(keys, KeyCount) << endl;
    for (int i = 0; i < 20; i++) {
        RandomShuffle();
    }
    Original();
    XYZSort();
    YZXSort();
    ZXYSort();
    CubeSort();
    FindSubCube();
    for (int i = 0; i < 20; i++) {
        SubCubeShuffle();
    }
    PrintResult();

    // SyntheticTrace();
    // for (int i = 0; i < 8; i++) {
    //     int k = 1 << i;
    //     Merge(k);
    //     print();
    // }
    // cout << "processed " << KeyCount << " keys of " << CurrentPointCloud << " point clouds" << endl; 
    // delete[] synkeys;
    // cout << "updated "<< totalOctreeInsertion << " nodes to Octree" << endl;
    return 0;
}