cmake_minimum_required(VERSION 3.20)
project(OS)

set(CMAKE_CXX_STANDARD 20)

# Copia '.env' al directorio de build al configurar
configure_file(${CMAKE_SOURCE_DIR}/.env ${CMAKE_BINARY_DIR}/.env COPYONLY)

# Archivos fuente SOLO de src/ - EXCLUYENDO indice_invertido/
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB HEADERS "include/*.h")

# Crear ejecutable principal OS - SOLO con archivos de src/
add_executable(OS ${SOURCES} ${HEADERS})

# Incluir la carpeta de headers
target_include_directories(OS PRIVATE ${CMAKE_SOURCE_DIR}/include)

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

# Agregar subdirectorio para la aplicación externa admin_tool
add_subdirectory(prog_extra/admin_tool)