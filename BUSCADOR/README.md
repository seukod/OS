# BUSCADOR SistOpe

Sistema de búsqueda distribuido con arquitectura de 3 capas usando sockets TCP.

## Arquitectura

```
MENÚ → BUSCADOR (cliente) → CACHE (servidor) → MOTOR_BUSQUEDA (servidor)
                                ↓                        ↓
                          [diccionario]         [índice invertido]
```

## Componentes

### 1. BUSCADOR (cliente)
- Ejecutable: `buscador/build/buscador`
- Función: Interfaz de usuario para realizar búsquedas
- Características:
  - Valida existencia del índice invertido
  - Conecta con servidor CACHE mediante socket
  - Muestra su PID
  - Realiza mapeo ID → nombre del libro usando MAPA-LIBROS.csv
  - Muestra resultados al usuario

### 2. CACHE (servidor persistente)
- Ejecutable: `cache/build/cache`
- Función: Capa de caché intermedia
- Características:
  - Servidor que escucha en puerto CACHE_PORT
  - Mantiene diccionario con tamaño CACHE_SIZE
  - Si encuentra dato en cache → respuesta directa
  - Si no encuentra → consulta al MOTOR_BUSQUEDA
  - Persiste entre múltiples búsquedas

### 3. MOTOR_BUSQUEDA (servidor persistente)
- Ejecutable: `motor_busqueda/build/motor_busqueda`
- Función: Motor de búsqueda sobre índice invertido
- Características:
  - Servidor que escucha en puerto MOTOR_PORT
  - Carga índice invertido en memoria (una sola vez)
  - Implementa algoritmo TOP-K
  - Devuelve respuestas en formato JSON

## Variables de Entorno

```bash
export CACHE_HOST=127.0.0.1
export CACHE_PORT=8080
export MOTOR_PORT=8081
export CACHE_SIZE=100
export TOPK=10
export INDICE_PATH=../build/indice_invertido.idx
```

## Compilación

```bash
# BUSCADOR
cd buscador
mkdir build && cd build
cmake ..
make

# CACHE
cd ../../cache
mkdir build && cd build
cmake ..
make

# MOTOR_BUSQUEDA
cd ../../motor_busqueda
mkdir build && cd build
cmake ..
make
```

## Uso

1. Iniciar servidores (una sola vez):
```bash
# Terminal 1: MOTOR_BUSQUEDA
./motor_busqueda/build/motor_busqueda

# Terminal 2: CACHE
./cache/build/cache
```

2. Realizar búsquedas (desde el menú principal):
```bash
# El menú llama a:
./buscador/build/buscador
```

## Formato JSON de Comunicación

```json
{
  "palabra": "ejemplo",
  "resultados": [
    {
      "id": 1,
      "frecuencia": 42,
      "posiciones": [10, 25, 37, 89, 120]
    },
    {
      "id": 3,
      "frecuencia": 15,
      "posiciones": [5, 50, 95]
    }
  ]
}
```

## Estado de Implementación

- [x] BUSCADOR: Estructura básica implementada
- [ ] BUSCADOR: Parser JSON completo
- [ ] CACHE: Por implementar
- [ ] MOTOR_BUSQUEDA: Por implementar
