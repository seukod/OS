cmake_minimum_required(VERSION 3.10)
project(admin_tool)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Archivos fuente necesarios para admin_tool
set(SOURCES
        src/admin_tool.cpp
        ../../src/interfaz.cpp
        ../../src/usuarios.cpp
        ../../src/users_auth.cpp
        ../../src/utils/input_utils.cpp
        ../../src/utils/string_utils.cpp
)

# Incluir directorios de headers
include_directories(
        include
        ../../include
)

# Crear ejecutable admin_tool
add_executable(admin_tool ${SOURCES})

# Directorio de salida del ejecutable
set_target_properties(admin_tool PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/prog_extra/admin_tool/bin"
        OUTPUT_NAME "admin_tool"
)



