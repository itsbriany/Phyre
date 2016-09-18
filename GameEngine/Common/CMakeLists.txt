cmake_minimum_required(VERSION 3.3)

project (Common)
include_directories($ENV{PROTOBUF_INCLUDE_DIR})
link_directories($ENV{PROTOBUF_LIBRARY_DIR})
add_library(Common Chat.pb.cc)
target_link_libraries(Common $ENV{PROTOBUF_LIBRARY_DIR}/libprotobuf.a)
