cmake_minimum_required(VERSION 3.6)

include_directories(${PHYRE_INCLUDE_DIRS})
file(GLOB configuration_SRC "*.h" "*.cpp")
add_library(Configuration ${configuration_SRC})
target_link_libraries(Configuration ${Boost_LIBRARIES} Logging)
set_target_properties(Configuration PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${PHYRE_LIBS}
                                               LIBRARY_OUTPUT_DIRECTORY ${PHYRE_LIBS})

# Visual Studio will not generate PDB file if we use the add_subdirectory trick
# and place the CMakeLists.txt file under the Test directory
include_directories(${PHYRE_INCLUDE_DIRS})
file(GLOB configuration_test_SRC "Test/*.h" "Test/*.cpp")
add_executable(ConfigurationTests ${configuration_test_SRC})
target_link_libraries(ConfigurationTests ${Boost_LIBRARIES} gmock gtest Configuration)
set_target_properties(ConfigurationTests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PHYRE_RUNTIME})
