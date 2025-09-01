cmake_minimum_required(VERSION 4.0)
project(OS)

set(CMAKE_CXX_STANDARD 20)

add_executable(OS main.cpp UsersFunc.cpp
        centro.h
        UI_Func.cpp
        UI_Func.h
        centro.cpp)