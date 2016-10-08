cmake_minimum_required(VERSION 3.3)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS} ../)
    add_library(XMPP xmpp_client.h xmpp_client.cpp sasl_handshake.h sasl_handshake.cpp)
    target_link_libraries(XMPP ${Boost_LIBRARIES} Networking)
endif()

add_subdirectory(Examples)
