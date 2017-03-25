cmake_minimum_required(VERSION 3.6)

include_directories(${PHYRE_INCLUDE_DIRS})
file(GLOB networking_SRC "*.h" "*.cpp")
add_library(Networking ${networking_SRC})
target_link_libraries(Networking ${Boost_LIBRARIES} Logging)

include_directories(${PHYRE_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
add_executable(NetworkingTests networking_tests.h fake_tcp_clients.h networking_test_runner.cpp)
target_link_libraries(NetworkingTests ${Boost_LIBRARIES} gmock gtest Networking)
