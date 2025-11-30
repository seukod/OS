cmake_minimum_required(VERSION 3.10)
project(buscador)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Archivos fuente
set(SOURCES
    src/main.cpp
    src/buscador.cpp
)

# Archivos de cabecera
include_directories(include)

# Crear ejecutable
add_executable(buscador ${SOURCES})

# Enlazar con bibliotecas necesarias (threads para sockets)
find_package(Threads REQUIRED)
target_link_libraries(buscador Threads::Threads)
