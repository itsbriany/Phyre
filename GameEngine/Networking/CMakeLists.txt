cmake_minimum_required(VERSION 3.3)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS} ../Logging)
    add_library(Networking host_resolver.h host_resolver.cpp tcp_socket.h tcp_socket.cpp tcp_client.h tcp_client.cpp)
    target_link_libraries(Networking ${Boost_LIBRARIES} Logging)
endif()

add_subdirectory(Examples)
add_subdirectory(XMPP)
