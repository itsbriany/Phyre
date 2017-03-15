cmake_minimum_required(VERSION 3.6)

project(GraphicsDemo)

include_directories(${PHYRE_INCLUDE_DIRS})

link_directories(${Boost_LIBRARY_DIRS} ${GLFW_LIBRARY_DIR})
add_executable(DrawCube draw_cube.cpp draw_cube.h)
target_link_libraries(DrawCube ${Boost_LIBRARIES}  ${VULKAN_LIBRARIES} glfw3 Logging Graphics)