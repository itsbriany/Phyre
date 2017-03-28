cmake_minimum_required(VERSION 3.6)

include_directories(${PHYRE_INCLUDE_DIRS})
file(GLOB configuration_SRC "*.h" "*.cpp")
add_library(Configuration ${configuration_SRC})
target_link_libraries(Configuration ${Boost_LIBRARIES} Logging)
set_target_properties(Configuration PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${PHYRE_LIBS}
                                               LIBRARY_OUTPUT_DIRECTORY ${PHYRE_LIBS})
add_subdirectory(Test)