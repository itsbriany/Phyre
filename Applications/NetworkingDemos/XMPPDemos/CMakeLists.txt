cmake_minimum_required(VERSION 3.3)

project (XMPPExamples)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS} ${PHYRE_XMPP})
    link_directories(${Boost_LIBRARY_DIRS} ${BASE64_LIBRARY_DIR})
    add_executable(XMPPClientDemo xmpp_client_demo.cpp)
    target_link_libraries(XMPPClientDemo ${Boost_LIBRARIES} XMPP)
endif()
