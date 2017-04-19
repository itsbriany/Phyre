cmake_minimum_required(VERSION 3.6)

include_directories(${PHYRE_INCLUDE_DIRS})
link_directories(${PHYRE_LIBRARY_DIRS})
file(GLOB input_SRC "*.h" "*.cpp")
add_library(Input ${input_SRC})
target_link_libraries(Input ${Boost_LIBRARIES} glfw Logging)
set_target_properties(Input PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${PHYRE_LIBS}
                                       LIBRARY_OUTPUT_DIRECTORY ${PHYRE_LIBS})
