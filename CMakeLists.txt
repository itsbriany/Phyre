cmake_minimum_required(VERSION 3.7)

project(Phyre)

# Check compiler version
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.5)
        message(FATAL_ERROR "Clang version must be at least 3.5!")
    endif()
endif()

# NOTE for Mac OS X users: You will want to set the environment
# variable DYLD_LIBRARY_PATH to point to you boost shared libraries (.dylib files).
# Example: export DYLD_LIBRARY_PATH=$BOOST_ROOT/stage/lib

# Set compiler options and preprocessor definitions

set(BASE64_DEFINITIONS "-DBUFFERSIZE=16777216")

if (WIN32)
    add_definitions(-D_WIN32_WINNT=0x601 -DGTEST_LINKED_AS_SHARED_LIBRARY=1 -D_SCL_SECURE_NO_WARNINGS -DVK_USE_PLATFORM_WIN32_KHR ${BASE64_DEFINITIONS})
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${BASE64_DEFINITIONS} -DBOOST_LOG_DYN_LINK -g -Wall -Wno-unused-function -Wno-deprecated-declarations -Wno-inconsistent-missing-override -std=c++14")
endif (WIN32)

# Compile with less warnings on MSVC
if(MSVC)
  message("Compiler Detected: MSVC")
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    message("Setting warning level /W1")
    string(REGEX REPLACE "/W[0-4]" "/W1" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    message("Setting warning level /W1")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W1")
  endif()

  include(ProcessorCount)
  ProcessorCount(N)
  message("Enabling parallel build with ${N} cores")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP${N}")
endif()

if (WIN32)
    set(Boost_USE_STATIC_LIBS ON)
endif()
set(Boost_USE_MULTITHREADED ON)

# For appveyor CI
if (WIN32)
    set(BOOST_LIBRARY_DIRS "$ENV{BOOST_LIBRARYDIR}")
    message("Boost library directory: ${BOOST_LIBRARY_DIRS}")
else()
    set(BOOST_LIBRARY_DIRS "${BOOST_ROOT}/stage/lib")
endif()


find_package(Boost 1.60 REQUIRED COMPONENTS system log thread random)

if(NOT Boost_FOUND)
    message(FATAL_ERROR "Could not find boost, please set the BOOST_ROOT environment variable to your boost installation directory")
endif()


# Where all third party dependencies reside
set(PHYRE_THIRD_PARTY "$ENV{PHYRE_ROOT}/ThirdParty")

# Add Gmock & Gtest libraries
set(TEST_ROOT "${PHYRE_THIRD_PARTY}/googletest")
set(GMOCK_INCLUDE_DIRS "${TEST_ROOT}/googlemock/include")
set(GMOCK_LIBRARY_DIRS "${TEST_ROOT}/Build/googlemock")

set(GTEST_INCLUDE_DIRS "${TEST_ROOT}/googletest/include")
set(GTEST_LIBRARY_DIRS "${GMOCK_LIBRARY_DIRS}/gtest")

# Add Protobuf
set(PROTOBUF_ROOT "$ENV{PROTOBUF_ROOT}")
set(PROTOBUF_INCLUDE_DIR "${PROTOBUF_ROOT}/src")
set(PROTOBUF_LIBRARY_DIR "${PROTOBUF_ROOT}/cmake")

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

# Add Base64
set(BASE64_ROOT "${PHYRE_THIRD_PARTY}/libb64-1.2")
set(BASE64_INCLUDE_DIR "${BASE64_ROOT}/include")
if (MSVC)
    set(BASE64_LIBRARY_DIR "${BASE64_ROOT}/base64/VisualStudioProject/x64")
else()
    set(BASE64_LIBRARY_DIR "${BASE64_ROOT}/src")
endif()

# Add GLFW
set(GLFW_ROOT "${PHYRE_THIRD_PARTY}/glfw")
set(GLFW_INCLUDE_DIR "${GLFW_ROOT}/include")
set(GLFW_LIBRARY_DIR "${GLFW_ROOT}/Build/src")

# Add GLM
set(GLM_ROOT "${PHYRE_THIRD_PARTY}/glm")
set(GLM_INCLUDE_DIR "${GLM_ROOT}")

# Expected build path
set(PHYRE_BUILD_PATH "$ENV{PHYRE_ROOT}/Build")

# Library paths
set(PHYRE_LIBS "$ENV{PHYRE_ROOT}/Libs")
set(PHYRE_LOGGING "${PHYRE_LIBS}/Logging")
set(PHYRE_NETWORKING "${PHYRE_LIBS}/Networking")
set(PHYRE_XMPP "${PHYRE_NETWORKING}/XMPP")
set(PHYRE_GRAPHICS "${PHYRE_LIBS}/Graphics")

# Vulkan SDK
set(VULKAN_ROOT "$ENV{VULKAN_ROOT}")
if (NOT VULKAN_ROOT)
    message(FATAL_ERROR "Could not find vulkan, please set the VULKAN_ROOT environment variable to point to the location of your Vulkan SDK installation")
    exit()
endif()
set(VULKAN_INCLUDE_DIR "${VULKAN_ROOT}/Include")
set(VULKAN_LIBRARY_DIR "${VULKAN_ROOT}/Lib")
file(GLOB VULKAN_LIBRARIES "${VULKAN_LIBRARY_DIR}/vulkan-1.lib")

# Vulkan C++ Bindings
set(VULKAN_CPP_BINDINGS "${PHYRE_THIRD_PARTY}/Vulkan-Hpp/vulkan")

# Testing paths
set(PHYRE_TESTING "${PHYRE_BUILD_PATH}/Testing")
set(PHYRE_TESTING_BIN_DIR "${PHYRE_TESTING}/Bin")

add_subdirectory(Libs)
add_subdirectory(Applications)
add_subdirectory(Testing)