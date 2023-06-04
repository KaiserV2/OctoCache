#ifndef CAST_RAY_H
#define CAST_RAY_H

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

using namespace std;
using namespace octomap;


void testCastRay(Cache* myCache, OcTree* tree) {
    point3d start[3] = {point3d(2.0,2.0,2.0), point3d(-2.0,-2.0,-2.0), point3d(-1.0,0.0,0.0)};
    point3d direction[3] = {point3d(1,1,1), point3d(-1,-1,-1), point3d(1,-1,-1)};
    double maxRange[10] = {0.2, 0.4, 0.6, 0.8, 1.0, 1.2, 1.4 ,1.6, 1.8, 2.0};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 10; k++) {
                point3d end;
                cout << tree->castRay(start[i], direction[j], end, true, maxRange[k]) << endl;
            }
        }
    }
    // point3d start(-1.0,0.0,0.0);
    // point3d direction(1,-1,-1);
    // double maxRange = 1.0;
    // point3d end;
    // cout << tree->castRay(start, direction, end, true, maxRange) << endl;
}

void testCastRay(OcTree* tree) {
    point3d start[3] = {point3d(2.0,2.0,2.0), point3d(-2.0,-2.0,-2.0), point3d(-1.0,0.0,0.0)};
    point3d direction[3] = {point3d(1,1,1), point3d(-1,-1,-1), point3d(1,-1,-1)};
    double maxRange[10] = {0.2, 0.4, 0.6, 0.8, 1.0, 1.2, 1.4 ,1.6, 1.8, 2.0};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 10; k++) {
                point3d end;
                cout << tree->castRay(start[i], direction[j], end, true, maxRange[k]) << endl;
            }
        }
    }
    // point3d start(-1.0,0.0,0.0);
    // point3d direction(1,-1,-1);
    // double maxRange = 1.0;
    // point3d end;
    // cout << tree->castRay(start, direction, end, true, maxRange) << endl;
}


#endif