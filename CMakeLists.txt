cmake_minimum_required(VERSION 3.9)
project(sokoban)

set(CMAKE_CXX_STANDARD 11)

add_library(sokoban SHARED library.cpp library.h)