cmake_minimum_required(VERSION 3.6)

include_directories(include)
file(GLOB b64_SRC "*.h" "src/*.c")
add_library(b64 ${b64_SRC})
add_executable(run_b64 ${b64_SRC} "base64/base64.cc")