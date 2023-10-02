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

#include <octomap/octomap.h>
#include <octomap/OcTree.h>
#include "castRay.h"
#include <chrono>

using namespace std;
using namespace octomap;

// #define USE_NEW_CACHE false

void print_time(std::string s) {
    auto currentTime = std::chrono::system_clock::now();

    auto duration = currentTime.time_since_epoch();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(duration);

    long long microsecCount = microseconds.count();

    std::cout << s << ":" << microsecCount << std::endl;
}

void print_query_info(point3d query, OcTreeNode* node) {
  if (node != NULL) {
    cout << "occupancy probability at " << query << ":\t " << node->getOccupancy() << endl;
  }
  else 
    cout << "occupancy probability at " << query << ":\t is unknown" << endl;    
}

int main(int /*argc*/, char** /*argv*/) {

  cout << endl;
  cout << "generating example map" << endl;

  print_time("start");

  OcTree* tree = new OcTree(0.1);  // create empty tree with resolution 0.1
  tree->myCache = new Cache(1<<10, 12, NULL, 1);
  delete tree;

// #if USE_NEW_CACHE
//   tree.myCache = new Cache(1<<10, 12, &tree, 1);
// #endif

//   // insert some measurements of occupied cells

//   for (int x=-20; x<20; x++) {
//     for (int y=-20; y<20; y++) {
//       for (int z=-20; z<20; z++) {
//         point3d endpoint ((float) x*0.05f, (float) y*0.05f, (float) z*0.05f);
// #if USE_NEW_CACHE
//         OcTreeKey key = tree.coordToKey(endpoint);
//         tree.myCache->updateNode(key, true); // integrate 'occupied' measurement
// #else
//         tree.updateNode(endpoint, true); // integrate 'occupied' measurement
// #endif
//       }
//     }
//   }

//   // insert some measurements of free cells

//   for (int x=-30; x<30; x++) {
//     for (int y=-30; y<30; y++) {
//       for (int z=-30; z<30; z++) {
//         point3d endpoint ((float) x*0.02f-1.0f, (float) y*0.02f-1.0f, (float) z*0.02f-1.0f);
// #if USE_NEW_CACHE
//         OcTreeKey key = tree.coordToKey(endpoint);
//         tree.myCache->updateNode(key, false);  // integrate 'free' measurement
// #else
//         tree.updateNode(endpoint, false);  // integrate 'free' measurement
// #endif
//       }
//     }
//   }


//   for (int i = 0; i <= 16; i++) {
//     int count = 0;
//     for (auto it = tree.begin(i); it != tree.end(); ++it) {
//       count++;
//     }
//     cout << i << " " << count << endl;
//   }
//   print_time("end");
//   cout << endl;
//   cout << "performing some queries:" << endl;
  
//   point3d query (0., 0., 0.);
//   OcTreeNode* result = tree.search (query);
//   print_query_info(query, result);

//   query = point3d(-1.,-1.,-1.);
//   result = tree.search (query);
//   print_query_info(query, result);

//   query = point3d(1.,1.,1.);
//   result = tree.search (query);
//   print_query_info(query, result);


//   cout << endl;
//   tree.writeBinary("simple_tree.bt");
//   cout << "wrote example file simple_tree.bt" << endl << endl;
//   cout << "now you can use octovis to visualize: octovis simple_tree.bt"  << endl;
//   cout << "Hint: hit 'F'-key in viewer to see the freespace" << endl  << endl;  

}
