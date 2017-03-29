cmake_minimum_required(VERSION 3.6)

# Build the graphics library
include_directories(${PHYRE_INCLUDE_DIRS})
link_directories(${PHYRE_LIBRARY_DIRS})
file(GLOB graphics_SRC "*.h" "*.cpp")
add_library(Graphics ${graphics_SRC})
target_link_libraries(Graphics ${Boost_LIBRARIES} ${VULKAN_LIBRARIES} glfw3 Logging)
set_target_properties(Graphics PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${PHYRE_LIBS}
                                          LIBRARY_OUTPUT_DIRECTORY ${PHYRE_LIBS})