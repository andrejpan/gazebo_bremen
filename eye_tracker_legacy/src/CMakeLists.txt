cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

add_library(eye_tracker SHARED EyeTracker.cc)
target_link_libraries(eye_tracker ${GAZEBO_LIBRARIES} ${MONGOCLIENT})

add_library(world_time_publisher SHARED WorldTimePublisher.cc)
target_link_libraries(world_time_publisher ${GAZEBO_LIBRARIES})
