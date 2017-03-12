cmake_minimum_required(VERSION 3.6)

include_directories(${PHYRE_INCLUDE_DIRS})
file(GLOB configuration_SRC "*.h" "*.cpp")
add_library(Configuration ${configuration_SRC})
target_link_libraries(Configuration ${Boost_LIBRARIES} Logging)