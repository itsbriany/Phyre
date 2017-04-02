cmake_minimum_required(VERSION 3.3)

include_directories(${PHYRE_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
add_executable(LoggableDemo loggable_example.cpp)
target_link_libraries(LoggableDemo ${Boost_LIBRARIES} Logging)
set_target_properties(LoggableDemo PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PHYRE_RUNTIME})
