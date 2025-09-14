cmake_minimum_required(VERSION 3.16)
project(crear_indice)

# Configurar estándar C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Incluir directorios de headers
include_directories(include)

# Definir archivos fuente
set(SOURCES
    src/main.cpp
    src/crear_indice.cpp
)

# Crear el ejecutable
add_executable(crear_indice ${SOURCES})

# Opciones de compilación
target_compile_options(crear_indice PRIVATE -Wall -Wextra)

# Configurar el directorio de salida
set_target_properties(crear_indice PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build
)

# Crear directorio build si no existe
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)
