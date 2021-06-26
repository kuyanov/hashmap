cmake_minimum_required(VERSION 3.17)
project(HashMap)

set(CMAKE_CXX_STANDARD 14)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options("-fsanitize=address")
    add_link_options("-fsanitize=address")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options("-Ofast")
endif()

add_executable(HashMap main.cpp)
