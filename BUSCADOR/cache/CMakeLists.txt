cmake_minimum_required(VERSION 3.10)
project(cache)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    src/main.cpp
    src/cache_server.cpp
)

include_directories(include)

add_executable(cache ${SOURCES})

find_package(Threads REQUIRED)
target_link_libraries(cache Threads::Threads)
