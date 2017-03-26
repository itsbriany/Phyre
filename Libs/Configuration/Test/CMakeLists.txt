cmake_minimum_required(VERSION 3.6)

include_directories(${PHYRE_INCLUDE_DIRS})
file(GLOB configuration_test_SRC "*.h" "*.cpp")
add_library(ConfigurationTests ${configuration_test_SRC})
target_link_libraries(ConfigurationTests ${Boost_LIBRARIES} gmock gtest Configuration)
set_target_properties(ConfigurationTests PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${PHYRE_LIBS}
                                                    LIBRARY_OUTPUT_DIRECTORY ${PHYRE_LIBS})
