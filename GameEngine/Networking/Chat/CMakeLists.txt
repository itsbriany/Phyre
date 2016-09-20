project (Chat)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS} ../../Logging)
    add_library(Chat chat_client.h chat_client.cpp host_resolver.h host_resolver_impl.h host_resolver_impl.cpp tcp_socket.h tcp_socket_impl.h tcp_socket_impl.cpp)
    target_link_libraries(Chat ${Boost_LIBRARIES} Logging)
endif()

add_subdirectory(Demo)
add_subdirectory(Test)
