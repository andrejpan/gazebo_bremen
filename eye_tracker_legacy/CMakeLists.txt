cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(eye_tracker_legacy)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GAZEBO_CXX_FLAGS} -Wall -std=c++11")

set(MONGOCLIENT /home/andrejpan/mongo-client-install/lib/libmongoclient.so)
set(MONGO_INCLUDE_DIR /home/andrejpan/mongo-client-install/include)

find_package(gazebo REQUIRED)

find_package(Protobuf REQUIRED)
set(PROTOBUF_IMPORT_DIRS)
foreach(ITR ${GAZEBO_INCLUDE_DIRS})
  if(ITR MATCHES ".*gazebo-[0-9.]+$")
    set(PROTOBUF_IMPORT_DIRS "${ITR}/gazebo/msgs/proto")
  endif()
endforeach()
set (msgs
  ${PROTOBUF_IMPORT_DIRS}/time.proto
)
PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ${msgs})

include_directories(${GAZEBO_INCLUDE_DIRS} 
                    ${PROJECT_SOURCE_DIR}/include 
                    ${MONGO_INCLUDE_DIR})
link_directories(${GAZEBO_LIBRARY_DIRS})

add_subdirectory(src)


