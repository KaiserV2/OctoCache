#ifndef GLOBALVARIABLES_OCTOMAP
#define GLOBALVARIABLES_OCTOMAP

#include "OcTree.h"

// variables retrieved from the original octomap code 
// only to make them globla variables

namespace octomap{
    extern bool lazy_eval;
    extern OcTree* tree;
}

#endif