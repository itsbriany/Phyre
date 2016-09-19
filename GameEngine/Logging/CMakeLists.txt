project (Logging)

find_package(Boost)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_library(Logging logger.cpp)
    target_link_libraries(Logging ${Boost_LIBRARIES})
endif()
