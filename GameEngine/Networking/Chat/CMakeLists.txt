cmake_minimum_required(VERSION 3.3)

project (Chat)
find_package(Boost)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS} ../../Logging)
    add_library(Chat chat_client.cpp host_resolver_impl.cpp tcp_socket_impl.cpp)
    target_link_libraries(Chat ${Boost_LIBRARIES} Logging)
endif()

add_subdirectory(Demo)
