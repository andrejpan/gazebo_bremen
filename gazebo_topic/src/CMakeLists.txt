cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

add_library(word_listener SHARED world_listener.cc)
target_link_libraries(word_listener ${GAZEBO_LIBRARIES})