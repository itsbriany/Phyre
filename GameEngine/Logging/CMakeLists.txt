project (Logging)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_library(Logging logging.h logger.h logger.cpp)
    target_link_libraries(Logging ${Boost_LIBRARIES})
endif()
