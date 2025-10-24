cmake_minimum_required(VERSION 3.20)
project(indice_invertido_paralelo)

# Configurar C++17 para usar std::filesystem
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Agregar directorios de includes
include_directories(include)

# Agregar archivos fuente
set(SOURCES
    src/main.cpp
    src/Crear_indice_inv.cpp
)

# Crear el ejecutable
add_executable(indice_invertido_paralelo ${SOURCES})

# Enlazar bibliotecas del sistema para filesystem
if(UNIX AND NOT APPLE)
    target_link_libraries(indice_invertido_paralelo stdc++fs)
endif()

# Para futuras implementaciones con hilos (ya que incluyes <thread>)
find_package(Threads REQUIRED)
target_link_libraries(indice_invertido_paralelo Threads::Threads)

# Configuraciones adicionales para depuración
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra")
