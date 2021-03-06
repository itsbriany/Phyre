cmake_minimum_required(VERSION 3.6)

project(Phyre)
include(cmake/vars.cmake)

option(DEBUG_BOOST "Enable this if cmake is having trouble finding your boost libraries" OFF)
option(CORES "Tweak this if you want more control over your parallel build, 0 means maximum cores" 0)
option(BOOST_LOG_DYN_LINK "Turn this ON if you are dynamically linking against boost log" OFF)

# Certain versions of clang do not support C++14
message("Asserting compatible compiler version...")
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.5)
        message(FATAL_ERROR "Clang version must be at least 3.5!")
    endif()
endif()

message("Enabling boost...")
# The runtime also determines how boost will be found.
# find_package Boost is very picky, so if you have not built the
# boost libraries conforming to these runtime configurations,
# it will say that it could not find the libraries.
if (BUILD_SHARED_LIBS OR BOOST_LOG_DYN_LINK)
    set(Boost_USE_STATIC_RUNTIME OFF)
    set(Boost_USE_STATIC_LIBS OFF)
else ()
    set(Boost_USE_STATIC_RUNTIME ON)
    set(Boost_USE_STATIC_LIBS ON)
endif()
set(Boost_USE_MULTITHREADED ON)
if (DEBUG_BOOST)
    set(Boost_DEBUG ON)
endif()
find_package(Boost REQUIRED COMPONENTS system log thread random)
if(NOT Boost_FOUND)
    message(FATAL_ERROR "Could not find boost, please set the BOOST_ROOT environment variable to your boost installation directory")
endif()

# Parallel Building
if (NOT CORES)
    include(ProcessorCount)
    ProcessorCount(CORES)
endif()
if (MSVC)
   message("Enabling parallel build with ${CORES} cores...")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP${CORES}")
else()
   message("Your CPU supports ${CORES} logical cores for parallel building")
endif()

message("Adding preprocessor definitions: ${PREPROCESSOR_DEFINITIONS}")
add_definitions(${PREPROCESSOR_DEFINITIONS})

set(USE_MSVC_RUNTIME_LIBRARY_DLL OFF CACHE BOOL "GLFW builds with /MD, we want /MT")

# Where all third party dependencies reside
set(PHYRE_THIRD_PARTY "ThirdParty")

# Add third party dependencies configure them
message("Adding Google Test...")
add_subdirectory(${PHYRE_THIRD_PARTY}/googletest)

message("Adding GLFW...")
option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_TESTS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
add_subdirectory(${PHYRE_THIRD_PARTY}/glfw)

message("Adding libb64...")
add_subdirectory(${PHYRE_THIRD_PARTY}/libb64-1.2)

# Bear in mind that when compiling C files, we are expected to set CMAKE_C_FLAGS
# instead of CMAKE_CXX_FLAGS!
message("Fixing default compiler settings...")
if (MSVC)
    # For MSVC, CMake sets certain flags to defaults we want to override.
    # This replacement code is taken from sample in the CMake Wiki at
    # http://www.cmake.org/Wiki/CMake_FAQ#Dynamic_Replace.
    foreach (FLAG_VAR
                CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
                CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO
                CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
                CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO)
        if (NOT BUILD_SHARED_LIBS)
        # Let's not depend on the computer having CRT DLLs; also, this makes it easy to link to gmock.lib
        string(REPLACE "/MD" "-MT" ${FLAG_VAR} "${${FLAG_VAR}}")
        endif()

        # Up the default warning level
        string(REPLACE "/W3" "/W4" ${FLAG_VAR} "${${FLAG_VAR}}")
    endforeach()
endif()

# Add Gmock & Gtest libraries
set(TEST_ROOT "${PHYRE_THIRD_PARTY}/googletest")
set(GMOCK_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/${TEST_ROOT}/googlemock/include")
set(GMOCK_LIBRARY_DIRS "${TEST_ROOT}/googlemock")

set(GTEST_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/${TEST_ROOT}/googletest/include")
set(GTEST_LIBRARY_DIRS "${GMOCK_LIBRARY_DIRS}/gtest")

# Add Protobuf
set(PROTOBUF_ROOT "$ENV{PROTOBUF_ROOT}")
set(PROTOBUF_INCLUDE_DIR "${PROTOBUF_ROOT}/src")
set(PROTOBUF_LIBRARY_DIR "${PROTOBUF_ROOT}/cmake")

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

# Add Base64
set(BASE64_ROOT "${PHYRE_THIRD_PARTY}/libb64-1.2")
set(BASE64_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/${BASE64_ROOT}/include")
if (MSVC)
    set(BASE64_LIBRARY_DIR "${BASE64_ROOT}")
else()
    set(BASE64_LIBRARY_DIR "${BASE64_ROOT}/src")
endif()

# Add GLFW
set(GLFW_ROOT "${PHYRE_THIRD_PARTY}/glfw")
set(GLFW_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/${GLFW_ROOT}/include")
set(GLFW_LIBRARY_DIR "${CMAKE_BINARY_DIR}/${GLFW_ROOT}/src")

# Add GLM
set(GLM_ROOT "${PHYRE_THIRD_PARTY}/glm")
set(GLM_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/${GLM_ROOT}")

# Vulkan SDK
if (NOT APPLE)
    message("Searching for a Vulkan SDK...")
    find_package(Vulkan)
    if (NOT Vulkan_FOUND)
        message("No Vulkan SDK found... Using the Khronos Vulkan SDK")
        set(VULKAN_ROOT "${PROJECT_SOURCE_DIR}/${PHYRE_THIRD_PARTY}/Vulkan-LoaderAndValidationLayers")
        set(VULKAN_INCLUDE_DIR "${VULKAN_ROOT}/include")
        set(VULKAN_LAYER_LIBRARY_DIR "${VULKAN_ROOT}/build/layers")
        set(VULKAN_JSON_LIBRARY_DIR "${VULKAN_ROOT}/build/libs/vkjson")
        set(VULKAN_LOADER_LIBRARY_DIR "${VULKAN_ROOT}/build/loader")
        set(GLSLANG_LIBRARY_DIR "${VULKAN_ROOT}/external/glslang/build/install")
        set(VULKAN_LIBRARY_DIRS ${VULKAN_LAYER_LIBRARY_DIR} ${VULKAN_JSON_LIBRARY_DIR} ${VULKAN_LOADER_LIBRARY_DIR} ${GLSLANG_LIBRARY_DIR})
        set(VULKAN_LIBRARY vulkan-1.lib)
        message("Vulkan SDK found: ${VULKAN_LIBRARY}")
    else()
        message("Vulkan SDK found: ${VULKAN_LIBRARY}")
    endif()
    # Vulkan C++ Bindings
    set(VULKAN_CPP_BINDINGS "${PROJECT_SOURCE_DIR}/${PHYRE_THIRD_PARTY}/Vulkan-Hpp/vulkan")
endif()

# VULKAN_LIBRARY is the absolute path tPhyo the Vulkan SDK; VULKAN_LIBRARIES is used
# when falling back to the Khronos Vulkan SDK when VULKAN_SDK could not be found
set(THIRD_PARTY_GRAPHICS_LIBRARIES ${VULKAN_LIBRARIES} ${VULKAN_LIBRARY} glfw)

# Library paths and output
set(PHYRE_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/Libs ${Boost_INCLUDE_DIRS}
                       ${GMOCK_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS}
                       ${BASE64_INCLUDE_DIR} ${GLFW_INCLUDE_DIR} ${GLM_INCLUDE_DIR}
                       ${VULKAN_CPP_BINDINGS} ${VULKAN_INCLUDE_DIR})
set(PHYRE_LIBRARY_DIRS ${Boost_LIBRARY_DIRS} ${GLFW_LIBRARY_DIR} ${VULKAN_LIBRARY_DIRS})

set(PHYRE_RUNTIME ${CMAKE_BINARY_DIR}/Bin)
set(PHYRE_LIBS ${CMAKE_BINARY_DIR}/Archive)

# We only care about warnings for this project
message("Setting warning flags...")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILER_WARNING_FLAGS}")

# Add the Phyre core components
message("Adding Libraries...")
add_subdirectory(Libs)

message("Adding Applications...")
add_subdirectory(Applications)

message("Adding Tests...")
add_subdirectory(Tests)
