cmake_minimum_required(VERSION 3.17)
project(hashmap)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS_ASAN "-g -fsanitize=address,undefined -fno-sanitize-recover=all")

add_executable(HashMap main.cpp)
