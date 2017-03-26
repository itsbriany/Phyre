cmake_minimum_required(VERSION 3.3)

# Library
include_directories(${PHYRE_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
file(GLOB XMPP_TESTS_SRC "*.cpp" "*.h")
add_library(XMPPTests ${XMPP_TESTS_SRC})
target_link_libraries(XMPPTests ${Boost_LIBRARIES} gmock gtest XMPP Configuration)
set_target_properties(XMPPTests PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${PHYRE_LIBS}
                                           LIBRARY_OUTPUT_DIRECTORY ${PHYRE_LIBS})
