cmake_minimum_required(VERSION 3.9)
project(sokoban)

set(CMAKE_CXX_STANDARD 11)

add_library(sokoban SHARED library.cpp library.h)
find_package( OpenCV REQUIRED )
find_package (Threads REQUIRED)
add_executable (sokoban_exe library.cpp play.cpp)
target_link_libraries( sokoban_exe ${OpenCV_LIBS} ${CMAKE_THREAD_LIBS_INIT})