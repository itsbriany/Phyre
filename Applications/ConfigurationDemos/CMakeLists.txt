cmake_minimum_required(VERSION 3.6)

include_directories(${PHYRE_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

add_executable(ConfigurationLoaderDemo configuration_loader_demo.cpp)
target_link_libraries(ConfigurationLoaderDemo ${Boost_LIBRARIES} Configuration)

message(My flags: ${CMAKE_CXX_FLAGS_DEBUG})