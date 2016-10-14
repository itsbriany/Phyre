cmake_minimum_required(VERSION 3.3)

project (NetworkingTests)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS} ../)
    link_directories(${GMOCK_LIBRARY_DIRS} ${GTEST_LIBRARY_DIRS} ${Boost_LIBRARY_DIRS})
    add_executable(NetworkingTests fake_tcp_clients.h tcp_client_tests.cpp)
    target_link_libraries(NetworkingTests ${Boost_LIBRARIES} gmock gtest Networking)
endif()
