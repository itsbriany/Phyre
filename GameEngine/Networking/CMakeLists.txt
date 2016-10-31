cmake_minimum_required(VERSION 3.3)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS} ../Logging)
    add_library(Networking host_resolver.h host_resolver.cpp
        tcp_server.h tcp_server.cpp tcp_socket.h tcp_socket.cpp
        tcp_client.h tcp_client.cpp tcp_server_connection.h
        tcp_server_connection.cpp)
    target_link_libraries(Networking ${Boost_LIBRARIES} Logging)

    # Tests
    # The executable must run with gmock.dll in the same directory
    if (WIN32)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${GMOCK_ROOT})
    endif()
    include_directories(${Boost_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS})
    link_directories(${GMOCK_LIBRARY_DIRS} ${GTEST_LIBRARY_DIRS} ${Boost_LIBRARY_DIRS})
    add_executable(NetworkingTests fake_tcp_clients.h networking_tests.cpp)
    target_link_libraries(NetworkingTests ${Boost_LIBRARIES} gmock gtest Networking)
    if (WIN32)
        unset(CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    endif()
endif()

add_subdirectory(Examples)
add_subdirectory(XMPP)
