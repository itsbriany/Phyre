cmake_minimum_required(VERSION 3.6)

# Bear in mind that when compiling C files, we are expected to set CMAKE_C_FLAGS
# instead of CMAKE_CXX_FLAGS!
message("Fixing default compiler settings...")
if (MSVC)
    # For MSVC, CMake sets certain flags to defaults we want to override.
    # This replacement code is taken from sample in the CMake Wiki at
    # http://www.cmake.org/Wiki/CMake_FAQ#Dynamic_Replace.
    foreach (FLAG_VAR
                CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
                CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO
                CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
                CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO)
        if (NOT BUILD_SHARED_LIBS)
        # Let's not depend on the computer having CRT DLLs; also, this makes it easy to link to gmock.lib
        string(REPLACE "/MD" "-MT" ${FLAG_VAR} "${${FLAG_VAR}}")
        endif()

        # Up the default warning level
        string(REPLACE "/W3" "/W4" ${FLAG_VAR} "${${FLAG_VAR}}")
    endforeach()
endif()

include_directories(include)
file(GLOB b64_SRC "*.h" "src/*.c")
add_library(b64 ${b64_SRC})
add_executable(run_b64 ${b64_SRC} "base64/base64.cc")