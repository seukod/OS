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

Además, se implementó un argumento de ayuda: 
- `-h`: Muestra un menú de ayuda donde aparece cómo ejecutar el programa

### Ejecución del multiplicador de matrices

El programa auxiliar para multiplicación de matrices se ejecuta de la siguiente manera:

```bash
./multi "<ruta_completa_archivo_A>" "<ruta_completa_archivo_B>" "<separador>"
```
Y contiene los siguientes parámetros:
- Primer argumento: Ruta completa del archivo A.TXT que contiene la primera matriz
- Segundo argumento: Ruta completa del archivo B.TXT que contiene la segunda matriz
- Tercer argumento: Carácter separador utilizado en las matrices

### 🧩 Funcionalidades Nuevas

---

## ✅ Administración de Usuarios y Perfiles

- Permite **crear**, **eliminar** y **listar** usuarios.
- Permite **crear**, **eliminar** y **listar** perfiles.
- Los usuarios se asocian a perfiles con roles específicos.
- No se permite eliminar el perfil `"ADMIN"` ni usuarios con ese perfil.
- ✔️ Funcionalidad completa e implementada.

---

## ✅ Multiplicación de Matrices NxN

- Permite cargar y multiplicar matrices cuadradas de tamaño `N x N`.
- Actualmente **no está conectada al sistema principal** (menú).
- ✔️ Funcionalidad lista, pendiente de integración.

---

## 🚧 Juego

- Mini-juego planificado dentro del sistema.
- En desarrollo.
- ❌ Funcionalidad en construcción.

---

## ✅ ¿Es palíndromo?

- Abre una interfaz para ingresar un texto.
- Opciones:
  1. **Validar** si el texto ingresado es o no un palíndromo.
  2. **Cancelar** para volver al menú principal.
- ✔️ Funcionalidad lista e implementada.

---

## ✅ Calcular f(x) = x² + 2x + 8

- Interfaz para ingresar un número real `x` y calcular `f(x) = x² + 2x + 8`.
- Muestra el resultado con detalle.
- Permite **calcular múltiples veces** hasta que el usuario decida volver.
- ✔️ Funcionalidad lista e implementada.

---

## ✅ Conteo sobre texto

- Interfaz para cargar un texto desde archivo.
- Entrega resumen de conteo con:
  - Cantidad de **vocales**
  - Cantidad de **consonantes**
  - Cantidad de **caracteres especiales**
  - Cantidad de **palabras**
- Incluye opción para **volver al menú principal**.
- ✔️ Funcionalidad lista e implementada.



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

### BOOKS_PATH
- **Propósito**: Ruta relativa hacia la carpeta de libros en formato .txt
- **Formato**: Prefijo de ruta para ser concatenado con el nombre del libro.txt a revisar
- **Valor actual**: `../data/libros/`


### Configuración Actual

Tu archivo `.env` está configurado así:
```dotenv
USERS_FILE = /Users/francisco/CLionProjects/OS/USUARIOS.txt
PROFILES_FILE = /Users/francisco/CLionProjects/OS/PERFILES.txt
