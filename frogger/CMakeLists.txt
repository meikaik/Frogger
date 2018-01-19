cmake_minimum_required(VERSION 3.9)
project(frogger)

set(CMAKE_CXX_STANDARD 14)

find_package(X11 REQUIRED)
link_libraries(${X11_LIBRARIES})
include_directories(${X11_INCLUDE_DIR})

add_executable(frogger main.cpp)