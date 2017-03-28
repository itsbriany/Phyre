cmake_minimum_required(VERSION 3.6)

include_directories(${PHYRE_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
file(GLOB NETWORKING_TESTS_SRC "*.cpp" "*.h")
add_executable(NetworkingTests ${NETWORKING_TESTS_SRC})
target_link_libraries(NetworkingTests ${Boost_LIBRARIES} gmock gtest Networking)
set_target_properties(NetworkingTests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PHYRE_RUNTIME})
