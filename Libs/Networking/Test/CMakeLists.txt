cmake_minimum_required(VERSION 3.6)

include_directories(${PHYRE_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
add_executable(NetworkingTests networking_tests.cpp fake_tcp_clients.h)
target_link_libraries(NetworkingTests ${Boost_LIBRARIES} gmock gtest Networking)
set_target_properties(NetworkingTests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PHYRE_RUNTIME})
