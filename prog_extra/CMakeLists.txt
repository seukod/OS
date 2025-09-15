cmake_minimum_required(VERSION 3.16)
project(prog_extra)

# Configurar estándar C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Incluir directorios de headers
include_directories(include)

# Definir archivos fuente
set(SOURCES
    source/multi_matrix.cpp
)

# Crear el ejecutable
add_executable(multi ${SOURCES})

# Opciones de compilación
target_compile_options(multi PRIVATE -Wall -Wextra)

# Configurar el directorio de salida (opcional)
set_target_properties(multi PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
