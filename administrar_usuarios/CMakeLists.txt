cmake_minimum_required(VERSION 3.20)
project(administrar_usuarios)

set(CMAKE_CXX_STANDARD 17)

# Configurar directorios
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/source)
set(MAIN_PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)

# Incluir directorios de cabeceras del proyecto principal
include_directories(${INCLUDE_DIR})
include_directories(${MAIN_PROJECT_DIR}/include)

# Archivos fuente de la aplicación
file(GLOB APP_SOURCES "${SOURCE_DIR}/*.cpp")

# Archivos fuente necesarios del proyecto principal
set(MAIN_SOURCES
    ${MAIN_PROJECT_DIR}/src/users_auth.cpp
    ${MAIN_PROJECT_DIR}/src/usuarios.cpp
    ${MAIN_PROJECT_DIR}/src/interfaz.cpp
    ${MAIN_PROJECT_DIR}/src/utils/input_utils.cpp
    ${MAIN_PROJECT_DIR}/src/utils/string_utils.cpp
)

# Crear ejecutable
add_executable(admin_usuarios ${APP_SOURCES} ${MAIN_SOURCES})

# Asegurar que el directorio data existe
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/data)
