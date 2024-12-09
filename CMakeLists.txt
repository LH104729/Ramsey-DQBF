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
# find_package(Boost REQUIRED)
# include_directories(${Boost_INCLUDE_DIRS})

file(GLOB SRC_COMMON "src/common/*.cpp")

add_executable(ramsey "src/main.cpp" ${SRC_COMMON})

include_directories(inc)

target_link_libraries(ramsey PRIVATE cxxopts::cxxopts)