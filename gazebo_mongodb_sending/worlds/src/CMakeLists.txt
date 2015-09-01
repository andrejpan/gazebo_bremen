cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

# instaled from https://github.com/mongodb/mongo-cxx-driver, legacy branch
set(MONGOCLIENT /home/andrejpan/mongo-client-install/lib/libmongoclient.so)
set(MONGO_INCLUDE_DIR /home/andrejpan/mongo-client-install/include)

include_directories( ${MONGO_INCLUDE_DIR} )
add_library(Client1 SHARED Client1.cc)
target_link_libraries(Client1 ${GAZEBO_LIBRARIES} ${Boost_LIBRARIES} ${PROTOBUF_LIBRARIES} ${MONGOCLIENT})
