cmake_minimum_required(VERSION 3.10)
project(motor_busqueda)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    src/main.cpp
)

include_directories(include)

add_executable(motor_busqueda ${SOURCES})

find_package(Threads REQUIRED)
target_link_libraries(motor_busqueda Threads::Threads)
