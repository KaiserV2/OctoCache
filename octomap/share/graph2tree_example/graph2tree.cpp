/*
 * OctoMap - An Efficient Probabilistic 3D Mapping Framework Based on Octrees
 * http://octomap.github.com/
 *
 * Copyright (c) 2009-2013, K.M. Wurm and A. Hornung, University of Freiburg
 * All rights reserved.
 * License: New BSD
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
#include "search.h"
#include "castRay.h"
#include "minmax.h"

using namespace std;
using namespace octomap;




void printUsage(char* self){
  std::cerr << "USAGE: " << self << " [options]\n\n";
  std::cerr << "This tool is part of OctoMap and inserts the data of a scan graph\n"
               "file (point clouds with poses) into an octree.\n"
               "The output is a compact maximum-likelihood binary octree file \n"
               "(.bt, bonsai tree) and general octree files (.ot) with the full\n"
               "information.\n\n";


  std::cerr << "OPTIONS:\n  -i <InputFile.graph> (required)\n"
            "  -o <OutputFile.bt> (required) \n"
            "  -res <resolution> (optional, default: 0.1 m)\n"
            "  -m <maxrange> (optional) \n"
            "  -n <max scan no.> (optional) \n"
            "  -log (enable a detailed log file with statistics) \n"
            "  -g (nodes are already in global coordinates and no transformation is required) \n"
            "  -compressML (enable maximum-likelihood compression (lossy) after every scan)\n"
            "  -simple (simple scan insertion ray by ray instead of optimized) \n"
            "  -discretize (approximate raycasting on discretized coordinates, speeds up insertion) \n"
            "  -clamping <p_min> <p_max> (override default sensor model clamping probabilities between 0..1)\n"
            "  -sensor <p_miss> <p_hit> (override default sensor model hit and miss probabilities between 0..1)"
  "\n";




  exit(0);
}

void SpecifyThread(){
    int s;
    pthread_t thread;
    cpu_set_t cpuset;
    thread = pthread_self();
    CPU_ZERO(&cpuset);
    CPU_SET(2, &cpuset);
    s = pthread_setaffinity_np(thread, sizeof(cpuset), &cpuset);
}

void calcThresholdedNodes(const OcTree* tree,
                          unsigned int& num_thresholded,
                          unsigned int& num_other)
{
  num_thresholded = 0;
  num_other = 0;

  for(OcTree::tree_iterator it = tree->begin_tree(), end=tree->end_tree(); it!= end; ++it){
    if (tree->isNodeAtThreshold(*it))
      num_thresholded++;
    else
      num_other++;
  }
}

void outputStatistics(const OcTree* tree){
  unsigned int numThresholded, numOther;
  calcThresholdedNodes(tree, numThresholded, numOther);
  size_t memUsage = tree->memoryUsage();
  unsigned long long memFullGrid = tree->memoryFullGrid();
  size_t numLeafNodes = tree->getNumLeafNodes();

  cout << "Tree size: " << tree->size() <<" nodes (" << numLeafNodes<< " leafs). " <<numThresholded <<" nodes thresholded, "<< numOther << " other\n";
  cout << "Memory: " << memUsage << " byte (" << memUsage/(1024.*1024.) << " MB)" << endl;
  cout << "Full grid: "<< memFullGrid << " byte (" << memFullGrid/(1024.*1024.) << " MB)" << endl;
  double x, y, z;
  tree->getMetricSize(x, y, z);
  cout << "Size: " << x << " x " << y << " x " << z << " m^3\n";
  cout << endl;
}


int main(int argc, char** argv) {
  // default values:
  double res = 0.1;
  string datasetname = "fr_079";
  string treeFilename = "output";
  uint32_t hashMapSize = 1 << 17;
  uint32_t bound = 4;
  uint32_t maxPCNum = 1;
  uint32_t clockSpeed = 1;
  string ss;
  string sk;
  double maxrange = -1;
  int max_scan_no = -1;
  bool detailedLog = false;
  bool simpleUpdate = false;
  bool discretize = false;
  bool dontTransformNodes = false;
  unsigned char compression = 1;



  // SpecifyThread();
  timeval start; 
  timeval stop;
  timeval stop1;  

  int arg = 0;
  while (++arg < argc) {
    if (! strcmp(argv[arg], "-i")){
      string graphFileNum = std::string(argv[++arg]);
      if (graphFileNum == "1"){
        datasetname = "fr_079";
        // hashMapSize = 1 << 17;
      }
      else if (graphFileNum == "2"){
        datasetname = "fr_campus";
        // hashMapSize = 1 << 22;
      }
      else if (graphFileNum == "3"){
        datasetname = "new_college";
        // hashMapSize = 1 << 13;
      }
      else{ // default
        datasetname = "fr_079";
        // hashMapSize = 1 << 17;
      }
    }
    else if (!strcmp(argv[arg], "-s")){ // specify the size of the hash table, in 2 to the power of the argument
      ss = std::string(argv[++arg]);
      hashMapSize = (1 << stoi(ss));
    }
    else if (! strcmp(argv[arg], "-k")){
      sk = std::string(argv[++arg]);
      clockSpeed *= stoi(sk);
    }
    else if (! strcmp(argv[arg], "-b")){
      string b = std::string(argv[++arg]);
      bound = stoi(b);
    }
    else if (! strcmp(argv[arg], "-maxPCNum")){
      string s = std::string(argv[++arg]);
      maxPCNum = stoi(s);
    }
    else if (!strcmp(argv[arg], "-o"))
      treeFilename = std::string(argv[++arg]);
    else if (! strcmp(argv[arg], "-res") && argc-arg < 2)
      printUsage(argv[0]);
    else if (! strcmp(argv[arg], "-res"))
      res = atof(argv[++arg]);
    else if (! strcmp(argv[arg], "-log"))
      detailedLog = true;
    else if (! strcmp(argv[arg], "-g"))
      dontTransformNodes = true;
    else if (! strcmp(argv[arg], "-simple"))
      simpleUpdate = true;
    else if (! strcmp(argv[arg], "-discretize"))
      discretize = true;
    else if (! strcmp(argv[arg], "-compress"))
      OCTOMAP_WARNING("Argument -compress no longer has an effect, incremental pruning is done during each insertion.\n");
    else if (! strcmp(argv[arg], "-compressML"))
      compression = 2;
    else if (! strcmp(argv[arg], "-m"))
      maxrange = atof(argv[++arg]);
    else if (! strcmp(argv[arg], "-n"))
      max_scan_no = atoi(argv[++arg]);
    else if (! strcmp(argv[arg], "-clamping") && (argc-arg < 3))
      printUsage(argv[0]);
    // else if (! strcmp(argv[arg], "-clamping")){
    //   clampingMin = atof(argv[++arg]);
    //   clampingMax = atof(argv[++arg]);
    // }
    else if (! strcmp(argv[arg], "-sensor") && (argc-arg < 3))
      printUsage(argv[0]);
    // else if (! strcmp(argv[arg], "-sensor")){
    //   probMiss = atof(argv[++arg]);
    //   probHit = atof(argv[++arg]);
    // }
    else {
      printUsage(argv[0]);
    }
  }
  string graphFilename = "/home/octomap/Dataset/" + datasetname + ".graph";
  if (graphFilename == "" || treeFilename == "") {
    printUsage(argv[0]);
  }


  // get default sensor model values:
  OcTree * tree = new OcTree(res);
  double clampingMin = tree->getClampingThresMin();
  double clampingMax = tree->getClampingThresMax();
  double probMiss = tree->getProbMiss();
  double probHit = tree->getProbHit();

  // verify input:
  if (res <= 0.0){
    OCTOMAP_ERROR("Resolution must be positive");
    exit(1);
  }

  if (clampingMin >= clampingMax || clampingMin < 0.0 || clampingMax > 1.0){
    OCTOMAP_ERROR("Error in clamping values:  0.0 <= [%f] < [%f] <= 1.0\n", clampingMin, clampingMax);
    exit(1);
  }

  if (probMiss >= probHit || probMiss < 0.0 || probHit > 1.0){
    OCTOMAP_ERROR("Error in sensor model (hit/miss prob.):  0.0 <= [%f] < [%f] <= 1.0\n", probMiss, probHit);
    exit(1);
  }

  std::string treeFilenameOT = treeFilename + ".ot";
  std::string treeFilenameMLOT = treeFilename + "_ml.ot";

  cout << "\nReading Graph file\n===========================\n";
  ScanGraph* graph = new ScanGraph();
  if (!graph->readBinary(graphFilename))
    exit(2);

  size_t num_points_in_graph = 0;
  if (max_scan_no > 0) {
    num_points_in_graph = graph->getNumPoints(max_scan_no-1);
    cout << "\n Data points in graph up to scan " << max_scan_no << ": " << num_points_in_graph << endl;
  }
  else {
    num_points_in_graph = graph->getNumPoints();
    cout << "\n Data points in graph: " << num_points_in_graph << endl;
  }

  // transform pointclouds first, so we can directly operate on them later
  if (!dontTransformNodes) {
    for (ScanGraph::iterator scan_it = graph->begin(); scan_it != graph->end(); scan_it++) {

      pose6d frame_origin = (*scan_it)->pose;
      point3d sensor_origin = frame_origin.inv().transform((*scan_it)->pose.trans());

      (*scan_it)->scan->transform(frame_origin);
      point3d transformed_sensor_origin = frame_origin.transform(sensor_origin);
      (*scan_it)->pose = pose6d(transformed_sensor_origin, octomath::Quaternion());

    }
  }


  std::ofstream logfile;
  if (detailedLog){
    logfile.open((treeFilename+".log").c_str());
    logfile << "# Memory of processing " << graphFilename << " over time\n";
    logfile << "# Resolution: "<< res <<"; compression: " << int(compression) << "; scan endpoints: "<< num_points_in_graph << std::endl;
    logfile << "# [scan number] [bytes octree] [bytes full 3D grid]\n";
  }



  cout << "\nCreating tree\n===========================\n";

  
  
#if USE_NEW_CACHE
  string filename = "/proj/softmeasure-PG0/Peiqing/Dataset/Octomap/OctreeInsertion/" + datasetname + ".txt";
  std::cout << "Running with Cache" << std::endl;
  std::cout << "Hash map size " << hashMapSize << std::endl;
  tree->myCache = new Cache(hashMapSize, bound, tree, clockSpeed);
#else 
  std::cout << "Running without Cache" << std::endl;
#endif

  tree->setClampingThresMin(clampingMin);
  tree->setClampingThresMax(clampingMax);
  tree->setProbHit(probHit);
  tree->setProbMiss(probMiss);

// #if USE_NEW_CACHE
//   testMinMax(tree->myCache, tree);
// #else 
//   testMinMax(tree);
// #endif
  
  // uint64_t point1 = __rdtsc();
  size_t numScans = graph->size();
  size_t currentScan = 1;
  fstream fout;
  fout.open("/proj/softmeasure-PG0/Peiqing/Test/distribution.txt");
  gettimeofday(&start, NULL);  // start timer
  for (ScanGraph::iterator scan_it = graph->begin(); scan_it != graph->end(); scan_it++) {
    if (max_scan_no > 0) cout << "("<<currentScan << "/" << max_scan_no << ") " << flush;
    // else cout << "("<<currentScan << "/" << numScans << ") " << flush;
    if (simpleUpdate)
      tree->insertPointCloudRays((*scan_it)->scan, (*scan_it)->pose.trans(), maxrange);
    else{
#if USE_CACHE | USE_NEW_CACHE
      tree->insertPointCloud((*scan_it)->scan, (*scan_it)->pose.trans(), tree->myCache, maxrange, false, discretize);
#else
      tree->insertPointCloud((*scan_it)->scan, (*scan_it)->pose.trans(), maxrange, false, discretize);
#endif
    }
    // cout << myCache->myHashMap.itemCount << endl;
    // myCache->myHashMap.itemCount = 0;  

    // if (compression == 2){
    //   tree->toMaxLikelihood();
    //   tree->prune();
    // }

    // if (detailedLog)
    //   logfile << currentScan << " " << tree->memoryUsage() << " " << tree->memoryFullGrid() << "\n";

    // if ((max_scan_no > 0) && (currentScan == (unsigned int) max_scan_no))
    //   break;
    currentScan++;
    // for (int i = 0; i < myCache->myHashMap.TABLE_SIZE; i++) {
    //   fout << myCache->myHashMap.table[i].size() << ",";
    // }
    // fout << endl;
  }
  gettimeofday(&stop, NULL);  // stop timer
#if DEBUG2
  cout << endl << myCache->bufferSize << endl;
#endif

// uint64_t point2 = __rdtsc();
#if DETAIL_COUNT
  cout << "fetch_from_octree " << fetch_from_octree << endl;
  cout << "insert_to_octree " << insert_to_octree << endl;
  cout << "insert_to_hashmap " << insert_to_hashmap << endl;
  cout << "insert_to_buffer " << insert_to_buffer << endl;
#endif

  double time_to_insert = (stop.tv_sec - start.tv_sec) + 1.0e-6 *(stop.tv_usec - start.tv_usec);
  cout << "Total run time " << time_to_insert << " sec " << endl;

  // fout.open("log.txt", std::ios_base::app);
  // fout << time_to_insert << ",";

  cout << "updated " << original_nodeupdate << " nodes in total"<< endl;
  // get rid of graph in mem before doing anything fancy with tree (=> memory)
  delete graph;
  if (logfile.is_open())
    logfile.close();


cout << "octree insertion time " << insert_time << endl;
cout << "duplicate check time " << duplicate_time << endl;
cout << "ray tracing time " << raytrace_time << endl;
#if USE_NEW_CACHE
cout << "cache miss " << fetch_from_octree << endl;
cout << "thread 2 octree time " << octree_time << endl;
cout << "countTotal " << countTotal << endl; 
cout << "kick time " << kick_time << endl;
#endif
// #if USE_NEW_CACHE
//   testMinMax(tree->myCache, tree);
// #else 
//   testMinMax(tree);
// #endif


#if USE_NEW_CACHE==false
  outputStatistics(tree);
#endif

  delete tree;

  return 0;
}
