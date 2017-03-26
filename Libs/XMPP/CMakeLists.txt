cmake_minimum_required(VERSION 3.3)

# Library
include_directories(${PHYRE_INCLUDE_DIRS})
file(GLOB XMPP_SRC "*.c*" "*.h")
add_library(XMPP ${XMPP_SRC})
target_link_libraries(XMPP ${Boost_LIBRARIES} Networking b64)
set_target_properties(XMPP PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PHYRE_LIBS}
                                      ARCHIVE_OUTPUT_DIRECTORY ${PHYRE_LIBS})

add_subdirectory(Test)
