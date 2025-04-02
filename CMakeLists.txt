cmake_minimum_required(VERSION 3.27)
project(TheoryCoinMiners)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)

add_executable(TheoryCoinMiners
        include/Node.h
        src/simon-tree.cpp
        src/Node.cpp
        include/shortlex.hpp
        src/shortlex.cpp
        src/Ranker.cpp
        include/Ranker.h
        main.cpp
        utils/RandomTextGenerator.h
)
