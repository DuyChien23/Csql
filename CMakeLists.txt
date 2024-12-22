cmake_minimum_required(VERSION 3.28)

set(This Csql)

project(${This} C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

enable_testing()

add_subdirectory(third_party)

add_subdirectory(src)
add_subdirectory(tests)