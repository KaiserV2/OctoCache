// $Id: simple.cpp 271 2011-08-19 10:02:26Z kai_wurm $

/**
* OctoMap:
* A probabilistic, flexible, and compact 3D mapping library for robotic systems.
* @author K. M. Wurm, A. Hornung, University of Freiburg, Copyright (C) 2009.
* @see http://octomap.sourceforge.net/
* License: New BSD License
*/

/*
 * Copyright (c) 2009, K. M. Wurm, A. Hornung, University of Freiburg
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the University of Freiburg nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <octomap/OcTree.h>
#include <octomap/OcTreeKey.h>

#include <iostream>
#include <string.h>
#include <fstream>
#include <algorithm>
#include <octomap/octomap_timing.h>

using namespace octomap;




std::string datasetname;
OcTree* tree = new OcTree(0.1);
std::ifstream fin;
// OcTreeKey keys[6265381];
// OcTreeKey keys[209517632];
OcTreeKey keys[317490869];
bool occupancy;
uint32_t pktCount = 0;
uint32_t pcCount = 0;
uint32_t lastpc = 0;
uint32_t firstpkt = 0;



bool ReadKey(std::ifstream& fin){
    std::string line;
    if (getline(fin, line)){
        if (line == "next") {
            if (getline(fin, line)){
                pcCount++;
                // std::cout << pcCount << std::endl;
                if (pcCount == lastpc){
                    firstpkt = pktCount;
                }
            }
            else{
                return false;
            }
        }
        uint32_t pos1 = line.find(" ", 0);
        uint32_t pos2 = line.find(" ", pos1 + 1);
        uint32_t pos3 = line.find(" ", pos2 + 1);
        
        std::string k1 = line.substr(0, pos1);
        std::string k2 = line.substr(pos1 + 1, pos2 - pos1 - 1);
        std::string k3 = line.substr(pos2 + 1, pos3 - pos2 - 1);
        std::string occ = line.substr(pos3 + 1, 1);
        keys[pktCount] = OcTreeKey(stoi(k1), stoi(k2), stoi(k3));
        pktCount++;
        occupancy = stoi(occ);
        return true;
    }
    else{
        return false;
    }
}

void shuffle_pointcloud(){
    
}



int main(int argc, char** argv) {
    int arg = 0;
    std::string graphFileNum;
    std::string mapSizeNum;
    std::string hashMapNum;
    std::string hashMapSize;

    while (++arg < argc) {
        if (! strcmp(argv[arg], "-i")){
            graphFileNum = std::string(argv[++arg]);
            if (graphFileNum == "1"){
                datasetname = "1";
                lastpc = 60;
            }
            else if (graphFileNum == "2"){
                datasetname = "2";
                lastpc = 80;
            }
            else if (graphFileNum == "3"){
                datasetname = "3";
                lastpc = 92000;
            }
            else{ // default
                datasetname = "1";
            }
        }
        if (! strcmp(argv[arg], "-s")){
            hashMapNum = std::string(argv[++arg]);
            if (hashMapNum == "4"){
                hashMapSize = "4";
            }
            else if (hashMapNum == "6"){
                hashMapSize = "6";
            }
            else if (hashMapNum == "8"){
                hashMapSize = "8";
            }
        }
    }
    std::string fileName;
    if(hashMapNum == ""){
        fileName = "/proj/softmeasure-PG0/Peiqing/Dataset/Octomap/OctreeInsertion/"+datasetname+".txt";
    }
    else{
        fileName = "/proj/softmeasure-PG0/Peiqing/Dataset/Octomap/OctreeInsertion/"+datasetname+"hash"+hashMapSize+".txt";
    }
    std::cout << fileName << std::endl;
    fin.open(fileName);
    while(ReadKey(fin)){

    }
    // std::random_shuffle(&keys[firstpkt], &keys[pktCount - 1]);
    timeval start; 
    timeval stop;
    for (int i = 0; i < pktCount; i++) {
        if(i == firstpkt){
            std::cout << firstpkt << std::endl;
            gettimeofday(&start, NULL);
        }
        tree->updateNode(keys[i], occupancy, false);
    }
    gettimeofday(&stop, NULL);
    double time_to_insert = (stop.tv_sec - start.tv_sec) + 1.0e-6 *(stop.tv_usec - start.tv_usec);
    std::cout << "Used " << time_to_insert << " time to inserted " << pktCount - firstpkt << " voxels" << std::endl; 
    // std::cout << pktCount << std::endl;
    return 0;
}