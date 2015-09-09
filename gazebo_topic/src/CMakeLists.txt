cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

add_library(world_listener SHARED world_listener.cc)
target_link_libraries(world_listener ${GAZEBO_LIBRARIES})

add_library(visual_listener SHARED visual_listener.cc)
target_link_libraries(visual_listener ${GAZEBO_LIBRARIES})
