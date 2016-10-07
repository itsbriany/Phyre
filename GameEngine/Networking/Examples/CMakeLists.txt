cmake_minimum_required(VERSION 3.3)

project (NetworkingExamples)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS} ../)
    link_directories(${Boost_LIBRARY_DIRS})
    add_executable(TCPClientDemo tcp_client_demo.cpp)
    target_link_libraries(TCPClientDemo ${Boost_LIBRARIES} Networking)
endif()
