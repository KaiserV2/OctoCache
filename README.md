OctoCache: Caching Voxels for Accelerating 3D Occupancy Mapping in Autonomous Systems
===========================================================================

OctoCache is a software cached system to accelerate the popular mapping system Octomap. The paper is submitted to ASPLOS 2025.

To activate OctoCache workflow, go to OctoCache/octomap/include/octomap/Param.h and set #define USE_NEW_CACHE true. On the contrary, setting #define USE_NEW_CACHE false gives the original Octomap workflow.

Below is the overview of octomap compilation. The compilation instructions remain the same for both Ocotomap and OctoCache.


OVERVIEW
--------

OctoMap consists of two separate libraries each in its own subfolder:
**octomap**, the actual library, and **octovis**, our visualization libraries and tools.
This README provides an overview of both, for details on compiling each please 
see [octomap/README.md](octomap/README.md) and [octovis/README.md](octovis/README.md) respectively.
See http://www.ros.org/wiki/octomap and http://www.ros.org/wiki/octovis if you 
want to use OctoMap in ROS; there are pre-compiled packages available.

You can build each library separately with CMake by running it from the subdirectories, 
or build octomap and octovis together from this top-level directory. E.g., to
only compile the library, run:

    cd octomap
    mkdir build
    cd build
    cmake ..
    make
  
To compile the complete package, run:

    cd build
    cmake ..
    make
  
Binaries and libs will end up in the directories `bin` and `lib` of the
top-level directory where you started the build.


See [octomap README](octomap/README.md) and [octovis README](octovis/README.md) for further
details and hints on compiling, especially under Windows.
