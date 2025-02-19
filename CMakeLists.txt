cmake_minimum_required(VERSION 3.27)
project(Ramsey)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(DEBUG "Enable debug mode" OFF)

if (DEBUG)
    add_definitions(-DDEBUG)
endif()

find_package(cxxopts CONFIG REQUIRED)

file(GLOB SRC_COMMON "src/common/*.cpp")
file(GLOB SRC_ABC_WRAPPER "src/abc_wrapper/*.cpp")
file(GLOB LIB_ABC "abc/build/libabc.a")

add_executable(ramsey "src/main.cpp" ${SRC_COMMON} ${SRC_ABC_WRAPPER})

include_directories(inc)
include_directories(abc/src)

target_link_libraries(ramsey PRIVATE cxxopts::cxxopts)
target_link_libraries(ramsey PRIVATE ${LIB_ABC})
target_link_libraries(ramsey PRIVATE edit)
