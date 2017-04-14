cmake_minimum_required (VERSION 3.1)

project(Star)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -std=c++11")

set(CMAKE_CXX_COMPLIER "clang++")

aux_source_directory(. DIR_SRCS)

add_executable(star ${DIR_SRCS})

target_link_libraries(star pthread)
