cmake_minimum_required(VERSION 3.3)

project (Common)

include_directories(${PROTOBUF_INCLUDE_DIR})
link_directories(${PROTOBUF_LIBRARY_DIR})
add_library(Common Chat.pb.h Chat.pb.cc)
target_link_libraries(Common protobuf)
