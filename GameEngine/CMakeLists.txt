cmake_minimum_required(VERSION 3.3)
# SET(CMAKE_CXX_FLAGS "-g -Wall -Werror -std=c++14")
# set(CMAKE_CXX_FLAGS "-g -Wall -std=c++14")

# NOTE for Mac OS X users: You will want to set the environment
# variable DYLD_LIBRARY_PATH to point to you boost shared libraries (.dylib files).
# Example: export DYLD_LIBRARY_PATH=$BOOST_ROOT/stage/lib
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBOOST_LOG_DYN_LINK -g -Wall -std=c++14")

set(BOOST_ROOT "$ENV{BOOST_ROOT}")
set(BOOST_INCLUDE_DIRS "${BOOST_ROOT}")
set(BOOST_LIBRARY_DIRS "${BOOST_ROOT}/stage/lib")

find_package(Boost 1.60 COMPONENTS thread log system)

# Add Gmock & Gtest libraries
set(GMOCK_ROOT "$ENV{GMOCK_ROOT}")
set(GMOCK_INCLUDE_DIRS "${GMOCK_ROOT}/include")
set(GMOCK_LIBRARY_DIRS "${GMOCK_ROOT}")

set(GTEST_ROOT "$ENV{GTEST_ROOT}")
set(GTEST_INCLUDE_DIRS "${GTEST_ROOT}/include")
set(GTEST_LIBRARY_DIRS "${GMOCK_ROOT}/gtest")


add_subdirectory(Common)
add_subdirectory(Logging)
add_subdirectory(Networking)
