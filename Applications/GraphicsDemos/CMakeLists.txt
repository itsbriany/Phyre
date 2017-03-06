cmake_minimum_required(VERSION 3.8)

project(GraphicsDemo)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS} ${PHYRE_GRAPHICS} ${PHYRE_LOGGING}
                        ${VULKAN_INCLUDE_DIR} ${VULKAN_CPP_BINDINGS}
                        ${GLFW_INCLUDE_DIR} ${GLM_INCLUDE_DIR})

    link_directories(${Boost_LIBRARY_DIRS} ${GLFW_LIBRARY_DIR})
    add_executable(DrawCube draw_cube.cpp draw_cube.h)
    target_link_libraries(DrawCube ${Boost_LIBRARIES}  ${VULKAN_LIBRARIES} glfw3 Logging Graphics)
endif()