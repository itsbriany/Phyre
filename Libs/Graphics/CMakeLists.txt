cmake_minimum_required(VERSION 3.8)

# Build the graphics library 
include_directories(${Boost_INCLUDE_DIRS} ${PHYRE_LOGGING} ${VULKAN_INCLUDE_DIR} ${VULKAN_CPP_BINDINGS}
                    ${GLFW_INCLUDE_DIR} ${GLM_INCLUDE_DIR})
link_directories(${GLFW_LIBRARY_DIR})
add_library(Graphics vulkan_debugger.h vulkan_debugger.cpp
            vulkan_window.h vulkan_window.cpp vulkan_swapchain.cpp vulkan_swapchain.h
            vulkan_gpu.h vulkan_gpu.cpp vulkan_device.h vulkan_device.cpp 
            vulkan_uniform_buffer.h vulkan_uniform_buffer.cpp
            vulkan_utils.h vulkan_utils.cpp
            vulkan_render_pass.h vulkan_render_pass.cpp vulkan_instance.h vulkan_instance.cpp geometry.h)
target_link_libraries(Graphics ${Boost_LIBRARIES} ${VULKAN_LIBRARYIES} glfw3 Logging)