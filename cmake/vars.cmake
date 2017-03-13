########################################################
# We may wish to ignore various compiler warnings
# We also want to set warnings as errors
if (MSVC)
    set(COMPILER_WARNING_FLAGS "/WX /wd4714 /wd4201 /wd4100")
else()
    set(COMPILER_WARNING_FLAGS "-Wall -Werror -Wno-unused-function -Wno-deprecated-declarations -Wno-inconsistent-missing-override")
endif()
########################################################
# Required for b64.lib
set(BASE64_DEFINITIONS -DBUFFERSIZE=16777216)
########################################################
if (MSVC)
    set(PREPROCESSOR_DEFINITIONS -D_WIN32_WINNT=0x601 -D_SCL_SECURE_NO_WARNINGS -DVK_USE_PLATFORM_WIN32_KHR ${BASE64_DEFINITIONS})
else()
    set(PREPROCESSOR_DEFINITIONS -DBOOST_LOG_DYN_LINK ${BASE64_DEFINITIONS})
endif()
########################################################
# Use C++ 14
set(CMAKE_CXX_STANDARD 14)
########################################################