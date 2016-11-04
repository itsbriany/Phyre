cmake_minimum_required(VERSION 3.3)

project(GameEngine)

# Check compiler version
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.5)
        message(FATAL_ERROR "Clang version must be at least 3.5!")
    endif()
endif()

# SET(CMAKE_CXX_FLAGS "-g -Wall -Werror -std=c++14")
# set(CMAKE_CXX_FLAGS "-g -Wall -std=c++14")

# NOTE for Mac OS X users: You will want to set the environment
# variable DYLD_LIBRARY_PATH to point to you boost shared libraries (.dylib files).
# Example: export DYLD_LIBRARY_PATH=$BOOST_ROOT/stage/lib

# Set compiler options and preprocessor definitions
if (WIN32)
    add_definitions(-D_WIN32_WINNT=0x601 -DGTEST_LINKED_AS_SHARED_LIBRARY=1 -D_SCL_SECURE_NO_WARNINGS)
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBOOST_LOG_DYN_LINK -g -Wall -Wno-unused-function -Wno-deprecated-declarations -Wno-inconsistent-missing-override -std=c++14")
endif (WIN32)

# Compile with less warnings on MSVC
if(MSVC)
  message("Compiler Detected: MSVC")
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    message("Setting warning level /W2")
    string(REGEX REPLACE "/W[0-4]" "/W2" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    message("Setting warning level /W2")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W2")
  endif()
endif()

if (WIN32)
    set(Boost_USE_STATIC_LIBS ON)
endif()
set(Boost_USE_MULTITHREADED ON)

set(BOOST_ROOT "$ENV{BOOST_ROOT}")
set(BOOST_INCLUDE_DIRS "${BOOST_ROOT}")
# For appveyor CI
if (WIN32)
    set(BOOST_LIBRARY_DIRS "$ENV{BOOST_LIBRARYDIR}")
    message("Boost library directory: ${BOOST_LIBRARY_DIRS}")
else()
    set(BOOST_LIBRARY_DIRS "${BOOST_ROOT}/stage/lib")
endif()


find_package(Boost 1.60 REQUIRED COMPONENTS system log thread)


# Add Gmock & Gtest libraries
set(GMOCK_ROOT "$ENV{GMOCK_ROOT}")
set(GMOCK_INCLUDE_DIRS "${GMOCK_ROOT}/include")
set(GMOCK_LIBRARY_DIRS "${GMOCK_ROOT}")


set(GTEST_ROOT "$ENV{GTEST_ROOT}")
set(GTEST_INCLUDE_DIRS "${GTEST_ROOT}/include")
set(GTEST_LIBRARY_DIRS "${GMOCK_ROOT}/gtest")


# Add Protobuf
set(PROTOBUF_ROOT "$ENV{PROTOBUF_ROOT}")
set(PROTOBUF_INCLUDE_DIR "${PROTOBUF_ROOT}/src")
set(PROTOBUF_LIBRARY_DIR "${PROTOBUF_ROOT}/cmake")

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

add_subdirectory(Common)
add_subdirectory(Logging)
add_subdirectory(Networking)
add_subdirectory(GameEngineTesting)