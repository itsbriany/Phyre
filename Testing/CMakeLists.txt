cmake_minimum_required(VERSION 3.3)
if(Boost_FOUND)
    set(TEST_DEPENDENCY_DIRS ${PHYRE_NETWORKING} ${PHYRE_XMPP} ${PHYRE_LOGGING})
    # The executable must run with gmock.dll in the same directory
    if (WIN32)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${GMOCK_LIBRARY_DIRS})
    endif()
    include_directories(${Boost_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS}
        ${GTEST_INCLUDE_DIRS} ${TEST_DEPENDENCY_DIRS} ${BASE64_INCLUDE_DIR})
    link_directories(${GMOCK_LIBRARY_DIRS} ${Boost_LIBRARY_DIRS}
        ${GTEST_LIBRARY_DIRS} ${TEST_DEPENDENCY_DIRS} ${BASE64_LIBRARY_DIR})
    add_executable(PhyreTests game_engine_tests.cpp)
    target_link_libraries(PhyreTests ${Boost_LIBRARIES} gmock gtest Networking XMPP)
    if (WIN32)
        unset(CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    endif()
endif()
