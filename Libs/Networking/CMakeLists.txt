cmake_minimum_required(VERSION 3.6)

include_directories(${PHYRE_INCLUDE_DIRS})
file(GLOB NETWORKING_SRC "*.h" "*.cpp")
add_library(Networking ${NETWORKING_SRC})
target_link_libraries(Networking ${Boost_LIBRARIES} Logging)
set_target_properties(Networking PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${PHYRE_LIBS}
                                            LIBRARY_OUTPUT_DIRECTORY ${PHYRE_LIBS})
add_subdirectory(Test)