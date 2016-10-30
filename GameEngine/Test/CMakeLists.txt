cmake_minimum_required(VERSION 3.3)

project (GameEngineTests)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS}
        ../Networking ../Logging)
    link_directories(${GMOCK_LIBRARY_DIRS} ${GTEST_LIBRARY_DIRS}
        ${Boost_LIBRARY_DIRS} ../Networking)
    add_executable(GameEngineTests game_engine_tests.cpp)
    target_link_libraries(GameEngineTests ${Boost_LIBRARIES} gmock gtest Networking)
endif()
