cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

add_library(Client1 SHARED Client1.cc)
target_link_libraries(Client1 ${GAZEBO_LIBRARIES} ${Boost_LIBRARIES} ${PROTOBUF_LIBRARIES})
