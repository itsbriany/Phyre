cmake_minimum_required(VERSION 3.3)

# Library
include_directories(${PHYRE_INCLUDE_DIRS})
add_library(XMPP xmpp_client.h xmpp_client.cpp xmpp_state.h sasl.h
    sasl.cpp xmpp_tests.h)
target_link_libraries(XMPP ${Boost_LIBRARIES} Networking b64)


include_directories(${PHYRE_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
add_executable(XMPPTests xmpp_tests.h xmpp_test_runner.cpp)
target_link_libraries(XMPPTests ${Boost_LIBRARIES} gmock gtest XMPP)
