cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

# instaled from https://github.com/mongodb/mongo-cxx-driver, 26compat
set(MONGOCLIENT /home/andrejpan/mongo-client-install-26compat/lib/libmongoclient.so)

add_library(Client1 SHARED Client1.cc)

MESSAGE('*******************************LINK LIBRARIES')
MESSAGE(${MONGOCLIENT})
MESSAGE(${GAZEBO_LIBRARIES})
#MESSAGE(${Boost_LIBRARIES}) # not working
MESSAGE(${PROTOBUF_LIBRARIES})

target_link_libraries(Client1 pthread mongoclient boost_thread boost_system boost_regex
${GAZEBO_LIBRARIES} ${PROTOBUF_LIBRARIES} ${MONGOCLIENT})
