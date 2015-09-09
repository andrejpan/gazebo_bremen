cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(topicListenerWord)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")

find_package(gazebo REQUIRED)
include_directories(${GAZEBO_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/include)
link_directories(${GAZEBO_LIBRARY_DIRS})
list(APPEND CMAKE_CXX_FLAGS "${GAZEBO_CXX_FLAGS}")

add_subdirectory(src)