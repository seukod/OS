cmake_minimum_required(VERSION 3.20)
project(OS)

set(CMAKE_CXX_STANDARD 20)

# Copia '.env' al directorio de build al configurar
configure_file(${CMAKE_SOURCE_DIR}/.env ${CMAKE_BINARY_DIR}/.env COPYONLY)

add_executable(OS 
    main.cpp 
    ArgParser.cpp 
    AppConfig.cpp
    interfaz.cpp
    usuarios.cpp
    crud.cpp
    crud.h
    interfaz.h
    usuarios.h
)

# Copia '.env' junto al binario tras compilar
add_custom_command(TARGET OS POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/.env
        $<TARGET_FILE_DIR:OS>/.env
)

# Fallback: ruta embebida al '.env' de la raíz
target_compile_definitions(OS PRIVATE
        DEFAULT_ENV_PATH="${CMAKE_SOURCE_DIR}/.env"
)