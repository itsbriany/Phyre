cmake_minimum_required(VERSION 3.3)

project (NetworkingDemos)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS} ${PHYRE_NETWORKING} ${PHYRE_LOGGING})
    link_directories(${Boost_LIBRARY_DIRS})

    add_executable(TCPServerDemo tcp_server_demo.cpp)
    target_link_libraries(TCPServerDemo ${Boost_LIBRARIES} Networking)

    add_executable(TCPClientDemo tcp_client_demo.cpp)
    target_link_libraries(TCPClientDemo ${Boost_LIBRARIES} Networking)
endif()

add_subdirectory(XMPPDemos)