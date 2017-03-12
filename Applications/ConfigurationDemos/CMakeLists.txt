cmake_minimum_required(VERSION 3.6)

include_directories(${PHYRE_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

add_executable(ResourceManagerDemo resource_manager_demo.cpp)
target_link_libraries(ResourceManagerDemo ${Boost_LIBRARIES} Configuration)