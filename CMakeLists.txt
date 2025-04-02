cmake_minimum_required(VERSION 3.27)
project(TheoryCoinMiners)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)

add_executable(TheoryCoinMiners
        include/Node.h
        src/simon-tree.cpp
        src/Node.cpp)
