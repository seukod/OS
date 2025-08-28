## Propósito de la aplicación

El propósito de este trabajo es desarrollar un módulo de sistema llamado **Administrador de Usuarios**. Este módulo, o proyecto, tiene como objetivo permitir la gestión básica de usuarios dentro de un sistema, implementando funcionalidades para crear, eliminar y listar usuarios. Todos los usuarios gestionados por el sistema serán almacenados en un archivo de texto denominado `USUARIOS.TXT`.

El desarrollo de este módulo busca poner en práctica conceptos del ramo "Sistemas operativos", tales como manejo de archivos, estructuras de datos y uso de variables de entorno. Además, se utiliza un struct para la representación de los usuarios y su lista en memoria.

Este trabajo busca simular un sistema real de administración de usuarios, diferenciando perfiles de acceso y asegurando la integridad de los datos mediante el uso de identificadores y validaciones.

 ### Prerrequisitos
- Compilador C++ (g++, clang++, etc.)
- CMake (opcional, para usar el sistema de build)

### Compilación y Ejecución

#### Usando CMake:
```bash
# Crear directorio de build
mkdir build && cd build

# Generar archivos de build
cmake ..

# Compilar
make

# Ejecutar
./OS
```

## Configuración de Variables de Entorno

### Archivo `.env`

La aplicación requiere un archivo `.env` ubicado en el directorio de ejecución (`cmake-build-debug/.env` para debug o raíz del proyecto para release). Este archivo debe contener las rutas absolutas a los archivos de datos.

### Variables Requeridas

#### USERS_FILE
- **Propósito**: Ruta al archivo que almacena los datos de usuarios
- **Formato**: Ruta absoluta al archivo CSV
- **Valor actual**: `/Users/francisco/CLionProjects/OS/USUARIOS.txt`
- **Estructura del archivo**: `ID,Nombre,Username,Password,Perfil`

#### PROFILES_FILE
- **Propósito**: Ruta al archivo que define los perfiles y sus funcionalidades
- **Formato**: Ruta absoluta al archivo de texto
- **Valor actual**: `/Users/francisco/CLionProjects/OS/PERFILES.txt`
- **Estructura del archivo**: `NombrePerfil;funcionalidad1,funcionalidad2,funcionalidadN`

### Configuración Actual

Tu archivo `.env` está configurado así:
```dotenv
USERS_FILE = /Users/francisco/CLionProjects/OS/USUARIOS.txt
PROFILES_FILE = /Users/francisco/CLionProjects/OS/PERFILES.txt
