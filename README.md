## Resumen del Programa

Este proyecto es un sistema operativo simulado desarrollado en C++ y Python que implementa múltiples funcionalidades para la gestión de usuarios, procesamiento de texto, juegos multijugador, análisis de rendimiento y búsqueda distribuida. El sistema utiliza conceptos de sistemas operativos como manejo de archivos, estructuras de datos, variables de entorno, threads y comunicación en red.

## Índice

- [Prerrequisitos](#prerrequisitos)
- [Compilación y Ejecución](#compilación-y-ejecución)
- [Arquitectura del Programa](#arquitectura-del-programa)
- [Administración de Usuarios y Perfiles](#administración-de-usuarios-y-perfiles-opción-1-del-menú)
- [Multiplicación de Matrices NxN](#multiplicación-de-matrices-nxn-opción-2-del-menú)
- [Calcular f(x) = x² + 2x + 8](#calcular-fx--x²--2x--8-opción-3-del-menú)
- [¿Es palíndromo?](#es-palíndromo-opción-4-del-menú)
- [Conteo sobre texto](#conteo-sobre-texto-opción-5-del-menú)
- [Índice Invertido](#índice-invertido-opciones-6-y-7-del-menú)
- [Battle City Multiplayer](#battle-city-multiplayer-opción-8-del-menú)
- [Análisis de Rendimiento](#análisis-de-rendimiento-opción-9-del-menú)
- [Sistema de Búsqueda (BUSCADOR)](#sistema-de-búsqueda-buscador-opción-10-del-menú)
- [Estructura General del Proyecto](#estructura-general-del-proyecto)
- [Configuración de Variables de Entorno](#configuración-de-variables-de-entorno)

### Prerrequisitos
- **Compilador C++**: g++, clang++, o similar (C++11 o superior)
- **CMake**: Para el sistema de build automatizado
- **Python 3.8+**: Para módulos de juego y análisis
- **Pygame**: Para el módulo de juego multijugador
- **Bibliotecas adicionales**: Según el módulo específico (detallado en cada sección)

### Compilación y Ejecución

#### Usando CMake:
```bash
# Crear directorio de build
mkdir build && cd build

# Generar archivos de build
cmake ..

# Compilar
make
```

### Ejecución del programa principal

El programa principal acepta argumentos de ejecución obligatorios:

```bash
./OS -u <usuario> -p <contraseña> -f <archivo>
```

**Argumentos:**
- `-u`: Nombre de usuario para autenticación
- `-p`: Contraseña del usuario
- `-f`: Archivo de texto a evaluar

---

## Arquitectura del Programa

El sistema operativo simulado sigue una arquitectura **modular y multi-lenguaje**, diseñada para integrar múltiples funcionalidades de manera eficiente y escalable.

### Arquitectura General
```
[Menú Principal (C++)]
    ├── Autenticación de Usuarios
    ├── Gestión de Perfiles y Permisos
    └── Coordinación de Módulos
        ├── Módulos en C++ (Núcleo del Sistema)
        │   ├── Administración de Usuarios
        │   ├── Procesamiento de Texto
        │   ├── Índice Invertido (Secuencial/Paralelo)
        │   ├── Análisis de Rendimiento
        │   └── Sistema de Búsqueda (BUSCADOR)
        └── Módulos en Python (Especializados)
            ├── Juego Multijugador (Battle City)
            └── Generación de Estadísticas y Gráficos
```

### Componentes Principales

#### 1. **Núcleo del Sistema (C++)**
- **Menú Principal**: Punto de entrada que coordina todas las funcionalidades según permisos de usuario
- **Sistema de Autenticación**: Validación de credenciales y gestión de sesiones
- **Gestión de Usuarios y Perfiles**: CRUD de usuarios con roles y permisos
- **Procesamiento de Texto**: Análisis léxico, conteo de elementos y validación de palíndromos
- **Índice Invertido**: Construcción de índices para búsqueda eficiente (versiones secuencial y paralela)
- **Sistema de Búsqueda Distribuido**: Arquitectura cliente-servidor con caché intermedia

#### 2. **Módulos Especializados (Python)**
- **Juego Multijugador**: Implementación completa de Battle City con networking TCP
- **Herramientas de Análisis**: Generación de estadísticas y gráficos de rendimiento

#### 3. **Sistema de Build**
- **CMake**: Sistema de compilación automatizado para módulos C++
- **Dependencias Externas**: Gestión de bibliotecas y entornos virtuales para Python

#### 4. **Configuración y Persistencia**
- **Variables de Entorno**: Configuración flexible sin recompilación
- **Archivos de Datos**: Persistencia de usuarios, perfiles, libros y logs
- **Sistema de Logs**: Registro de operaciones y métricas de rendimiento

### Características Arquitecturales

- **Modularidad**: Cada funcionalidad es independiente pero integrada
- **Multi-lenguaje**: C++ para rendimiento, Python para prototipado rápido
- **Escalabilidad**: Arquitectura distribuida para búsqueda y juegos
- **Seguridad**: Control de acceso basado en roles y permisos
- **Flexibilidad**: Configuración mediante variables de entorno
- **Mantenibilidad**: Separación clara de responsabilidades

---

## Administración de Usuarios y Perfiles (Opción 1 del menú)

- Permite **crear**, **eliminar** y **listar** usuarios.
- Permite **crear**, **eliminar** y **listar** perfiles.
- Los usuarios se asocian a perfiles con roles específicos.
- No se permite eliminar el perfil `"ADMIN"` ni usuarios con ese perfil.
- Funcionalidad completa e implementada.

---

## Multiplicación de Matrices NxN (Opción 2 del menú)

- Permite cargar y multiplicar matrices cuadradas de tamaño `N x N`.
  Y contiene los siguientes parámetros:
- Primer argumento: Ruta completa del archivo A.TXT que contiene la primera matriz
- Segundo argumento: Ruta completa del archivo B.TXT que contiene la segunda matriz
- Tercer argumento: Carácter separador utilizado en las matrices



---

## Calcular f(x) = x² + 2x + 8 (Opción 3 del menú)

- Interfaz para ingresar un número real `x` y calcular `f(x) = x² + 2x + 8`.
- Muestra el resultado con detalle.
- Permite **calcular múltiples veces** hasta que el usuario decida volver.


---

## ¿Es palíndromo? (Opción 4 del menú)

- Abre una interfaz para ingresar un texto.
- Opciones:
  1. **Validar** si el texto ingresado es o no un palíndromo.
  2. **Cancelar** para volver al menú principal.


---

## Conteo sobre texto (Opción 5 del menú)

- Interfaz para cargar un texto desde archivo.
- Entrega resumen de conteo con:
  - Cantidad de **vocales**
  - Cantidad de **consonantes**
  - Cantidad de **caracteres especiales**
  - Cantidad de **palabras**
- Incluye opción para **volver al menú principal**.
- Funcionalidad lista e implementada.

---

## Índice Invertido (Opciones 6 y 7 del menú)

- **Dos implementaciones disponibles**:
  - **Opción 6**: Versión secuencial - Procesamiento tradicional sin paralelización
  - **Opción 7**: Versión paralela - Usa múltiples threads para acelerar el procesamiento
- **Funcionalidad**: Construye un índice invertido a partir de una colección de archivos de texto.
- **Características**:
  - Tokenización e indexación de documentos
  - Paralelización para máquinas con varios núcleos
  - Integración para un sistema de búsqueda (BUSCADOR)
- **Estructura del módulo**:
  - `indice_invertido/`: Versión secuencial
  - `indice_invertido_paralelo/`: Versión paralela con threads


---

## Battle City Multiplayer (Opción 8 del menú)

- **Juego: Battle City** con soporte para hasta 4 jugadores simultáneos.
- **Modos de juego**:
  - **Hostear partida**: Crear servidor y esperar conexiones (Jugador 1 - Verde)
  - **Unirse a partida**: Conectarse a un servidor existente (Jugadores 2-4)
- **Implementado en Python** usando Pygame para gráficos y networking.
- **Características**:
  - Controles: Flechas para movimiento, Espacio para disparar
  - Sistema de lobby: Presionar 'R' para marcar listo
  - Colores diferenciados por equipos y jugador
  - Estadísticas de juego (victorias, muertes, disparos)
- **Estructura del módulo**:
  - `Juego/game_multiplayer.py`: Cliente principal del juego
  - `Juego/server_dedicated.py`: Servidor dedicado para hosting
  - `Juego/network.py`: Capa de red con sockets TCP
  - `Juego/maps.py`: Definición de mapas y terrenos
  - `Juego/game_stats.py`: Sistema de estadísticas
  - `Juego/test_network.py`: Pruebas de conectividad
- **Requisitos**: Python 3.8+, Pygame instalado


---

## Análisis de Rendimiento (Opción 9 del menú)

- **Herramienta de benchmarking** para medir el rendimiento de algoritmos paralelizados.
- **Funcionalidad**: Ejecuta el generador de índice invertido con diferentes números de threads y mide tiempos de ejecución.
- **Características**:
  - Genera logs de rendimiento automáticamente
  - Crea gráficos comparativos usando Python y matplotlib
  - Configurable mediante variables de entorno
- **Estructura del módulo**:
  - `analisis/analisis_threads.cpp`: Programa de benchmarking en C++
  - `analisis/grafico_rendimiento.py`: Generador de gráficos en Python
- **Variables de entorno**:
  - `ANALISIS_LOG_PATH`: Ruta del archivo de logs (por defecto: logs/analisis_threads.log)
  - `ANALISIS_IMG_PATH`: Directorio para guardar gráficos (por defecto: graficos/)


---

## Sistema de Búsqueda (BUSCADOR) (Opción 10 del menú)

- **Sistema de búsqueda distribuido** con arquitectura de 3 capas usando sockets.
- **Arquitectura**:
  ```
  MENÚ → BUSCADOR (cliente) → CACHE (servidor) → MOTOR_BUSQUEDA (servidor)
                                  ↓                        ↓
                            [diccionario]         [índice invertido]
  ```
- **Componentes**:
  - **BUSCADOR (cliente)**: Interfaz de usuario para búsquedas
  - **CACHE (servidor)**: Capa intermedia de caché persistente
  - **MOTOR_BUSQUEDA (servidor)**: Motor de búsqueda sobre índice invertido
- **Características**:
  - Comunicación mediante JSON
  - Algoritmo TOP-K para resultados
  - Integración completa con el índice invertido
- **Variables de entorno requeridas**:
  - `CACHE_HOST`, `CACHE_PORT`, `MOTOR_PORT`
  - `CACHE_SIZE`, `TOPK`, `INDICE_PATH`
- **Estructura del módulo**:
  - `BUSCADOR/buscador/`: Cliente de búsqueda
  - `BUSCADOR/cache/`: Servidor de caché
  - `BUSCADOR/motor_busqueda/`: Motor de búsqueda

---

## Estructura General del Proyecto

Este proyecto está organizado de manera **modular**, con cada funcionalidad independiente pero integrada a través del **menú principal** (centro de activación). La arquitectura permite:

- **Módulos independientes**: Cada funcionalidad tiene su propio directorio con código fuente y build.
- **Integración centralizada**: El menú principal (`src/menus/menu_principal.cpp`) coordina todas las funcionalidades según permisos de usuario.
- **Configuración flexible**: Variables de entorno permiten adaptar rutas y parámetros sin recompilar.
- **Multi-lenguaje**: C++ para el núcleo del sistema, Python para módulos específicos (juego, análisis).

### Directorios principales:
- `src/`: Código fuente principal del sistema operativo simulado
- `include/`: Headers y definiciones de interfaces
- `build/`: Archivos de compilación (generados por CMake)
- `data/`: Archivos de datos (usuarios, libros, etc.)
- `administrar_usuarios/`: Módulo de gestión de usuarios
- `indice_invertido*/`: Implementaciones del índice invertido
- `BUSCADOR/`: Sistema de búsqueda distribuido
- `Juego/`: Módulo de juego multijugador
- `analisis/`: Herramientas de benchmarking
- `prog_extra/`: Utilidades adicionales (multiplicación de matrices)

---

## Configuración de Variables de Entorno

### Archivo `.env`

La aplicación requiere un archivo `.env` ubicado en el directorio de ejecución (`cmake-build-debug/.env` para debug o raíz del proyecto para release). Este archivo debe contener las rutas a los archivos de datos.

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

### BOOKS_PATH
- **Propósito**: Ruta relativa hacia la carpeta de libros en formato .txt
- **Formato**: Prefijo de ruta para ser concatenado con el nombre del libro.txt a revisar
- **Valor actual**: `../data/libros/`

### Variables adicionales para módulos específicos:
- `ANALISIS_LOG_PATH`: Para el módulo de análisis de rendimiento
- `ANALISIS_IMG_PATH`: Para gráficos de rendimiento
- `CACHE_HOST`, `CACHE_PORT`, `MOTOR_PORT`: Para el sistema BUSCADOR
- `CACHE_SIZE`, `TOPK`, `INDICE_PATH`: Para configuración del BUSCADOR

### Configuración Actual

Tu archivo `.env` está configurado así:
```dotenv
USERS_FILE = /Users/francisco/CLionProjects/OS/USUARIOS.txt
PROFILES_FILE = /Users/francisco/CLionProjects/OS/PERFILES.txt
BOOKS_PATH = ../data/libros/
```
