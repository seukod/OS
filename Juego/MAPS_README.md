# 🗺️ Sistema de Mapas Configurables - Battle City Multiplayer

## 📁 Archivos

- `maps.py` - Configuración centralizada de mapas
- `game_multiplayer.py` - Cliente del juego (importa mapas)
- `network.py` - Servidor (importa mapas)

## 🎮 Mapas Disponibles

### 1. **MAP_DEFAULT** (Mapa por defecto)
Diseño equilibrado con muros dispersos y espacios abiertos.

### 2. **MAP_MAZE** (Laberinto)
Mapa denso con muchos muros formando un laberinto.

### 3. **MAP_ARENA** (Arena abierta)
Espacio mayormente abierto con obstáculos mínimos.

## ⚙️ Cómo Cambiar de Mapa

### Método 1: Editar `maps.py`

Abre `maps.py` y cambia la variable `CURRENT_MAP`:

```python
# En maps.py, línea ~100
CURRENT_MAP = MAP_DEFAULT   # ← Cambiar aquí

# Opciones:
CURRENT_MAP = MAP_DEFAULT   # Mapa por defecto
CURRENT_MAP = MAP_MAZE      # Laberinto
CURRENT_MAP = MAP_ARENA     # Arena abierta
```

## 🎨 Crear Tu Propio Mapa

### Paso 1: Define la matriz

Cada número representa una celda de 32x32 pixels:

```python
# En maps.py
MI_MAPA_CUSTOM = [
    # 25 columnas x 18 filas
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0],
    # ... 16 filas más ...
]
```

### Paso 2: Actívalo

```python
CURRENT_MAP = MI_MAPA_CUSTOM
```

### Paso 3: Reinicia el juego

Cierra y vuelve a abrir tanto el servidor como los clientes.

## 📐 Dimensiones

| Elemento | Tamaño | Detalles |
|----------|--------|----------|
| **Pantalla** | 800x600 px | Tamaño fijo |
| **Tile** | 32x32 px | TILE_SIZE constante |
| **Matriz** | 25 x 18 | (800/32) x (600/32) |
| **Celdas totales** | 450 | 25 * 18 |

## 🎯 Leyenda de Celdas

| Valor | Significado | Visual |
|-------|-------------|--------|
| `0` | Espacio vacío | Transitable |
| `1` | Muro destructible | 🧱 Ladrillo naranja |

## 📍 Posiciones de Spawn

Configuradas en `SPAWN_POSITIONS`:

```python
SPAWN_POSITIONS = [
    (1, 1),   # Jugador 1 - Arriba izquierda (Equipo Azul)
    (1, 23),  # Jugador 2 - Arriba derecha (Equipo Azul)
    (16, 1),  # Jugador 3 - Abajo izquierda (Equipo Rojo)
    (16, 23), # Jugador 4 - Abajo derecha (Equipo Rojo)
]
```

**Formato:** `(fila, columna)` en la matriz

**Cálculo de coordenadas:**
- `x = columna * 32`
- `y = fila * 32`

## 💡 Consejos de Diseño

### ✅ Buenos Diseños

- **Simetría**: Mapas balanceados para ambos equipos
- **Espacios abiertos**: Permiten movimiento táctico
- **Refugio**: Algunos muros para protección
- **Pasillos**: Rutas alternativas

### ❌ Evitar

- Bloquear completamente las posiciones de spawn
- Mapas completamente vacíos (aburridos)
- Mapas completamente llenos (imposibles de jugar)
- Asimetría extrema (desequilibrio)

## 🖼️ Visualizador ASCII

Puedes visualizar tu mapa antes de probarlo:

```python
# Agregar al final de maps.py
def visualize_map(map_matrix):
    """Muestra el mapa en consola"""
    for row in map_matrix:
        line = ""
        for cell in row:
            line += "█" if cell == 1 else " "
        print(line)

# Usar:
visualize_map(CURRENT_MAP)
```

## 🔄 Ejemplo Completo: Crear Mapa Custom

```python
# 1. Editar maps.py
MAP_FORTRESS = [
    [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
    [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
    [1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1],
    [1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1],
    [1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1],
    [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
    [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
    [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
    [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
    [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
    [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
    [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
    [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
    [1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1],
    [1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1],
    [1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1],
    [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
    [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
]

# 2. Activar
CURRENT_MAP = MAP_FORTRESS

# 3. Probar visualización (opcional)
visualize_map(MAP_FORTRESS)
```

**Resultado visual:**
```
█████████████████████████
█                       █
█  ███         ███      █
█  █ █         █ █      █
█  ███         ███      █
█          ██           █
█          ██           █
█                       █
█                       █
█          ██           █
█          ██           █
█                       █
█                       █
█  ███         ███      █
█  █ █    ██   █ █      █
█  ███    ██   ███      █
█                       █
█████████████████████████
```

## 🔧 Troubleshooting

### Problema: Los muros no aparecen

**Solución**: Verifica que guardaste `maps.py` y reiniciaste el servidor.

### Problema: Error de importación

**Solución**: Asegúrate de que `maps.py` está en la misma carpeta que `game_multiplayer.py` y `network.py`.

### Problema: El mapa se ve diferente en cliente vs servidor

**Solución**: Ambos deben importar el mismo `CURRENT_MAP`. Reinicia ambos.

## 🚀 Flujo de Trabajo Recomendado

```bash
# 1. Diseñar mapa
$ nano maps.py  # o tu editor favorito

# 2. Visualizar (opcional)
$ python3 -c "from maps import CURRENT_MAP, visualize_map; visualize_map(CURRENT_MAP)"

# 3. Probar en juego
$ python3 game_multiplayer.py
```

---

¡Ahora puedes crear niveles personalizados fácilmente! 🎮✨
