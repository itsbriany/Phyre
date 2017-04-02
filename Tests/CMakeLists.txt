cmake_minimum_required(VERSION 3.6)

include_directories(${PHYRE_INCLUDE_DIRS})
file(GLOB TESTS_SRC "*.h" "*.cpp")
add_executable(PhyreTests ${TESTS_SRC})
target_link_libraries(PhyreTests ${Boost_LIBRARIES} gtest gmock Utils XMPP)
set_target_properties(PhyreTests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PHYRE_RUNTIME})