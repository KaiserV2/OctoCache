CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT( octomap-test )

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}-mavx2 -O3 -g")

find_package(octomap REQUIRED)
include_directories(${OCTOMAP_INCLUDE_DIRS})
link_directories(${OCTOMAP_LIBRARY_DIRS})
link_libraries(${OCTOMAP_LIBRARIES})

# ADD_EXECUTABLE(graph2tree graph2tree.cpp)

ADD_EXECUTABLE(tmp tmp.cpp)
