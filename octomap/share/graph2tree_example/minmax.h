#ifndef MIN_MAX_H
#define MIN_MAX_H

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

void testMinMax(Cache* myCache, OcTree* tree){
    cout << tree->changed() << endl;
    double x, y ,z;
    tree->getMetricMin(x,y,z);
    cout << "min: " << x << " " << y << " " << z << endl;
    tree->getMetricMax(x,y,z);
    cout << "max: " << x << " " << y << " " << z << endl;
}

void testMinMax(OcTree* tree){
    cout << tree->changed() << endl;
    double x, y ,z;
    tree->getMetricMin(x,y,z);
    cout << "min: " << x << " " << y << " " << z << endl;
    tree->getMetricMax(x,y,z);
    cout << "max: " << x << " " << y << " " << z << endl;
}


#endif