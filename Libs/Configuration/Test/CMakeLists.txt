cmake_minimum_required(VERSION 3.6)

include_directories(${PHYRE_INCLUDE_DIRS})
file(GLOB configuration_test_SRC "*.h" "*.cpp")
add_executable(ConfigurationTests ${configuration_test_SRC})
target_link_libraries(ConfigurationTests ${Boost_LIBRARIES} gmock gtest Configuration)