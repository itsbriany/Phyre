cmake_minimum_required(VERSION 3.3)

project (ChatClientDemo)
find_package(Boost)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS} ../../../Logging ../)
    add_executable(ChatClientDemo chat_client_demo.cpp)
    target_link_libraries(ChatClientDemo ${Boost_LIBRARIES} Chat Logging)
endif()
