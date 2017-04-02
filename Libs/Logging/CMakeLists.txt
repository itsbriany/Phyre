project (Logging)

include_directories(${Boost_INCLUDE_DIRS})
add_library(Logging loggable_interface.h log_level.h logging.h logger.h logger.cpp)
target_link_libraries(Logging ${Boost_LIBRARIES})
set_target_properties(Logging PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${PHYRE_LIBS}
                                         LIBRARY_OUTPUT_DIRECTORY ${PHYRE_LIBS})
