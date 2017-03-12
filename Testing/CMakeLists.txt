cmake_minimum_required(VERSION 3.3)
if(Boost_FOUND)
    include_directories(${PHYRE_INCLUDE_DIRS})
    link_directories(${GMOCK_LIBRARY_DIRS} ${Boost_LIBRARY_DIRS}
        ${GTEST_LIBRARY_DIRS} ${TEST_DEPENDENCY_DIRS} ${BASE64_LIBRARY_DIR})
    add_executable(PhyreTests phyre_tests.cpp)
    target_link_libraries(PhyreTests ${Boost_LIBRARIES} gmock Networking XMPP)
endif()
