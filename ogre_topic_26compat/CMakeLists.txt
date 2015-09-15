cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(topicListenerWord)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GAZEBO_CXX_FLAGS} -Wall -std=c++11")

set(MONGO_INCLUDE_DIR /home/andrejpan/mongo-client-install-26compat/include)
set(MONGOCLIENT /home/andrejpan/mongo-client-install-26compat/lib/libmongoclient.so)

find_package(gazebo REQUIRED)

include_directories(${GAZEBO_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/include ${MONGO_INCLUDE_DIR})
link_directories(${GAZEBO_LIBRARY_DIRS})

add_subdirectory(src)
