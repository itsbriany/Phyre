project (Chat)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS} ../ ../../Logging)
    add_library(Chat chat_client.h chat_client.cpp)
    target_link_libraries(Chat ${Boost_LIBRARIES} Logging Networking)
endif()

add_subdirectory(Demo)
add_subdirectory(Test)
