cmake_minimum_required(VERSION 3.3)

project (LoggingExamples)
if(Boost_FOUND)
    include_directories(${PHYRE_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})

    add_executable(LoggableDemo loggable_example.cpp)
    target_link_libraries(LoggableDemo ${Boost_LIBRARIES} Logging)
endif()
