cmake_minimum_required(VERSION 3.3)

# Build the graphics library
include_directories(${Boost_INCLUDE_DIRS} ${PHYRE_LOGGING} ${Vulkan_INCLUDE_DIRS} ${VULKAN_CPP_BINDINGS})
add_library(Graphics rendering_system_interface.h vulkan_rendering_system.h vulkan_rendering_system.cpp)
target_link_libraries(Graphics ${Boost_LIBRARIES} ${Vulkan_LIBRARIES} Logging)

# The executable must run with gmock.dll in the same directory
if (WIN32)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PHYRE_TESTING_BIN_DIR})
endif()
include_directories(${Boost_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS} ${TEST_DEPENDENCY_DIRS})
link_directories(${GMOCK_LIBRARY_DIRS} ${Boost_LIBRARY_DIRS} ${GTEST_LIBRARY_DIRS} ${TEST_DEPENDENCY_DIRS})
add_executable(GraphicsTests graphics_tests.h graphics_test_runner.cpp)
target_link_libraries(GraphicsTests ${Boost_LIBRARIES} ${Vulkan_LIBRARIES} gmock gtest Graphics)
if (WIN32)
    unset(CMAKE_RUNTIME_OUTPUT_DIRECTORY)
endif()