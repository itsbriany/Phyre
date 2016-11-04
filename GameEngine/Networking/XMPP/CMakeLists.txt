cmake_minimum_required(VERSION 3.3)

if(Boost_FOUND)
    # Library
    include_directories(${Boost_INCLUDE_DIRS} ../)
    add_library(XMPP xmpp_client.h xmpp_client.cpp xmpp_tests.h)
    target_link_libraries(XMPP ${Boost_LIBRARIES} Networking)

    # The executable must run with gmock.dll in the same directory
    if (WIN32)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${GMOCK_ROOT})
    endif()
    include_directories(${Boost_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS} ${TEST_DEPENDENCY_DIRS})
    link_directories(${GMOCK_LIBRARY_DIRS} ${Boost_LIBRARY_DIRS} ${GTEST_LIBRARY_DIRS} ${TEST_DEPENDENCY_DIRS})
    add_executable(XMPPTests xmpp_tests.h xmpp_test_runner.cpp)
    target_link_libraries(XMPPTests ${Boost_LIBRARIES} gmock gtest XMPP)
    if (WIN32)
        unset(CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    endif()
endif()

add_subdirectory(Examples)
