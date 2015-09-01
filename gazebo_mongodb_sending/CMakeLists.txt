cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(testing)

# instaled from https://github.com/mongodb/mongo-cxx-driver, legacy branch
set(MONGOCLIENT /home/andrejpan/mongo-client-install/lib/libmongoclient.so)
set(MONGO_INCLUDE_DIR /home/andrejpan/mongo-client-install/include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#set the default path for built executables to the "build" directory
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build)
#set the default path for built libraries to the "lib" directory
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build)

find_package(gazebo REQUIRED)

## Find Gazebo package
include (FindPkgConfig)
if (PKG_CONFIG_FOUND)
  pkg_check_modules(GAZEBO gazebo)
  pkg_check_modules(OGRE OGRE)
  pkg_check_modules(OGRE-Terrain OGRE-Terrain)
endif()

include_directories(
    ${GAZEBO_INCLUDE_DIRS}
    ${OGRE_INCLUDE_DIRS}
    ${OGRE-Terrain_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/include
    ${MONGO_INCLUDE_DIR})
link_directories(${GAZEBO_LIBRARY_DIRS} ${OGRE_LIBRARY_DIRS} ${MONGOCLIENT})

add_subdirectory(src)
