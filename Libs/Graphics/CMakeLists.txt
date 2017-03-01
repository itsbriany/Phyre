cmake_minimum_required(VERSION 3.8)

# Build the graphics library 
include_directories(${Boost_INCLUDE_DIRS} ${PHYRE_LOGGING} ${Vulkan_INCLUDE_DIRS} ${VULKAN_CPP_BINDINGS}
                    ${GLFW_INCLUDE_DIR} ${GLM_INCLUDE_DIR})
link_directories(${GLFW_LIBRARY_DIR})
add_library(Graphics rendering_system_interface.h vulkan_errors.h vulkan_errors.cpp vulkan_debugger.h vulkan_debugger.cpp
            vulkan_rendering_system.h vulkan_rendering_system.cpp vulkan_swapchain.h vulkan_swapchain.cpp vulkan_window.h vulkan_window.cpp
            vulkan_loader.h vulkan_loader.cpp vulkan_gpu.h vulkan_gpu.cpp device_manager.h device_manager.cpp 
            command_buffer_manager.h command_buffer_manager.cpp vulkan_uniform_buffer.h vulkan_uniform_buffer.cpp
            vulkan_memory_manager.h vulkan_memory_manager.cpp vulkan_pipeline.h vulkan_pipeline.cpp 
            vulkan_render_pass.h vulkan_render_pass.cpp vulkan_instance.h vulkan_instance.cpp geometry.h)
target_link_libraries(Graphics ${Boost_LIBRARIES} ${Vulkan_LIBRARIES} glfw3 Logging)

if (WIN32)
    # The executable must run with gmock.dll in the same directory
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PHYRE_TESTING_BIN_DIR})
endif()
include_directories(${Boost_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS} ${TEST_DEPENDENCY_DIRS})
link_directories(${GMOCK_LIBRARY_DIRS} ${Boost_LIBRARY_DIRS} ${GTEST_LIBRARY_DIRS} ${TEST_DEPENDENCY_DIRS})
add_executable(GraphicsTests graphics_tests.h graphics_test_runner.cpp)
target_link_libraries(GraphicsTests ${Boost_LIBRARIES} ${Vulkan_LIBRARIES} gmock gtest Graphics)
if (WIN32)
    set_target_properties(GraphicsTests PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PHYRE_TESTING_BIN_DIR}")
    unset(CMAKE_RUNTIME_OUTPUT_DIRECTORY)
endif()