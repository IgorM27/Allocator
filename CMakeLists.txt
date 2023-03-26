cmake_minimum_required(VERSION 3.23)
project(labwork-9-IgorM27)

set(CMAKE_CXX_STANDARD 23)

include_directories(include)

add_executable(labwork-9-IgorM27 main.cpp
               include/Pool.h
               include/Allocator.h)
