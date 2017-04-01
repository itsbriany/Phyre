cmake_minimum_required(VERSION 3.6)

include_directories(${PHYRE_INCLUDE_DIRS})
link_directories(${PHYRE_LIBRARY_DIRS})
add_executable(DrawCube draw_cube.cpp draw_cube.h)
target_link_libraries(DrawCube ${Boost_LIBRARIES} ${THIRD_PARTY_GRAPHICS_LIBRARIES} Logging Graphics)
set_target_properties(DrawCube PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PHYRE_RUNTIME})