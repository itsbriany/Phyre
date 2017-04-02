cmake_minimum_required(VERSION 3.3)

include_directories(${PHYRE_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

add_executable(TCPServerDemo tcp_server_demo.cpp)
target_link_libraries(TCPServerDemo ${Boost_LIBRARIES} Networking)
set_target_properties(TCPServerDemo PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PHYRE_RUNTIME})

add_executable(TCPClientDemo tcp_client_demo.cpp)
target_link_libraries(TCPClientDemo ${Boost_LIBRARIES} Networking)
set_target_properties(TCPClientDemo PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PHYRE_RUNTIME})
