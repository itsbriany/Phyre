cmake_minimum_required(VERSION 3.3)

include_directories(${PHYRE_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
add_executable(XMPPClientDemo xmpp_client_demo.cpp)
target_link_libraries(XMPPClientDemo ${Boost_LIBRARIES} XMPP)
set_target_properties(XMPPClientDemo PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PHYRE_RUNTIME})
