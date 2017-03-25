########################################################
# We may wish to ignore various compiler warnings
# We also want to set warnings as errors
if (MSVC)
    set(COMPILER_WARNING_FLAGS "/WX /wd4714 /wd4201 /wd4100")
else()
    set(COMPILER_WARNING_FLAGS "-Wall -Wextra -Werror")
endif()
########################################################
# Required for b64.lib
set(BASE64_DEFINITIONS -DBUFFERSIZE=16777216)
########################################################
if (MSVC)
    set(PREPROCESSOR_DEFINITIONS -D_WIN32_WINNT=0x601 -D_SCL_SECURE_NO_WARNINGS -DVK_USE_PLATFORM_WIN32_KHR ${BASE64_DEFINITIONS})
else()
    set(PREPROCESSOR_DEFINITIONS ${BASE64_DEFINITIONS})
endif()
########################################################
# Use C++ 14
set(CMAKE_CXX_STANDARD 14)
########################################################
# Set compiler flags
if (NOT MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
endif()
