cmake_minimum_required(VERSION 3.6)

include_directories(${PHYRE_INCLUDE_DIRS})
file(GLOB UTILS_SRC "*.h" "*.cpp")
add_library(Utils ${UTILS_SRC})
target_link_libraries(Utils ${Boost_LIBRARIES} Logging Configuration)
set_target_properties(Utils PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${PHYRE_LIBS}
                                       LIBRARY_OUTPUT_DIRECTORY ${PHYRE_LIBS})