cmake_minimum_required(VERSION 3.3)

project (Common)

set(PROTOBUF_ROOT "$ENV{PROTOBUF_ROOT}")
set(PROTOBUF_INCLUDE_DIR "${PROTOBUF_ROOT}/src")
set(PROTOBUF_LIBRARY_DIR "${PROTOBUF_ROOT}/cmake")

include_directories($ENV{PROTOBUF_INCLUDE_DIR})
link_directories($ENV{PROTOBUF_LIBRARY_DIR})
add_library(Common Chat.pb.cc)
target_link_libraries(Common $ENV{PROTOBUF_LIBRARY_DIR}/libprotobuf.a)
