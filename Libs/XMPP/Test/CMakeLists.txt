cmake_minimum_required(VERSION 3.3)

# Library
include_directories(${PHYRE_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
add_executable(XMPPTests xmpp_tests.cpp)
target_link_libraries(XMPPTests ${Boost_LIBRARIES} gmock gtest XMPP Configuration)
set_target_properties(XMPPTests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PHYRE_RUNTIME})
