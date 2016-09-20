cmake_minimum_required(VERSION 3.3)

project (ChatTests)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS} ../)
    link_directories(${GMOCK_LIBRARY_DIRS} ${GTEST_LIBRARY_DIRS} ${Boost_LIBRARY_DIRS})
    add_executable(ChatTests mock_host_resolver.h mock_tcp_socket.h chat_tests.cpp)
    target_link_libraries(ChatTests ${Boost_LIBRARIES} gmock gtest Chat)
endif()
