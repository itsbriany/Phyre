cmake_minimum_required(VERSION 3.3)

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
    add_definitions(-D_WIN32_WINNT=0x601 -DGTEST_LINKED_AS_SHARED_LIBRARY=1 -D_SCL_SECURE_NO_WARNINGS ${BASE64_DEFINITIONS})
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

set(BOOST_ROOT "$ENV{BOOST_ROOT}")
set(BOOST_INCLUDE_DIRS "${BOOST_ROOT}")

# For appveyor CI
if (WIN32)
    set(BOOST_LIBRARY_DIRS "$ENV{BOOST_LIBRARYDIR}")
    message("Boost library directory: ${BOOST_LIBRARY_DIRS}")
else()
    set(BOOST_LIBRARY_DIRS "${BOOST_ROOT}/stage/lib")
endif()


find_package(Boost 1.60 REQUIRED COMPONENTS system log thread random)


# Add Gmock & Gtest libraries
set(TEST_ROOT "$ENV{PHYRE_ROOT}/ThirdParty/googletest")
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
set(BASE64_ROOT "$ENV{PHYRE_ROOT}/ThirdParty/libb64-1.2")
set(BASE64_INCLUDE_DIR "${BASE64_ROOT}/include")
if (MSVC)
    set(BASE64_LIBRARY_DIR "${BASE64_ROOT}/base64/VisualStudioProject")
else()
    set(BASE64_LIBRARY_DIR "${BASE64_ROOT}/src")
endif()

# Library paths
set(PHYRE_LIBS $ENV{PHYRE_ROOT}/Libs)
set(PHYRE_LOGGING ${PHYRE_LIBS}/Logging)
set(PHYRE_NETWORKING ${PHYRE_LIBS}/Networking)
set(PHYRE_XMPP ${PHYRE_NETWORKING}/XMPP)

add_subdirectory(Libs)
add_subdirectory(Applications)
add_subdirectory(Testing)