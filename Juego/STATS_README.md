# 📊 Sistema de Estadísticas del Juego - Battle City Multiplayer

## 📋 Descripción

Este sistema genera automáticamente **4 gráficos estadísticos** a partir de los logs del juego:

1. **Disparos por equipo en cada partida** - Barras comparativas por partida
2. **Victorias por equipo (total acumulado)** - Barras con porcentajes
3. **Muertes por jugador** - Ranking de jugadores más eliminados
4. **Duración promedio de partidas** - Línea temporal con promedio

## 📁 Archivos

- `game_stats.py` - Programa principal de análisis
- `logs/game_*.log` - Archivos de log del juego (generados automáticamente)
- `stats_output/` - Carpeta de salida de gráficos (configurable)

## 🚀 Uso Rápido

### Opción 1: Carpeta por defecto

```bash
cd Juego
python3 game_stats.py
```

Los gráficos se guardarán en `stats_output/`

### Opción 2: Carpeta personalizada (variable de entorno)

```bash
export GAME_STATS_OUTPUT_DIR=/ruta/personalizada/graficos
python3 game_stats.py
```

O en una sola línea:

```bash
GAME_STATS_OUTPUT_DIR=~/Desktop/estadisticas python3 game_stats.py
```

### Opción 3: Configuración permanente

Agregar a tu `~/.zshrc` o `~/.bash_profile`:

```bash
export GAME_STATS_OUTPUT_DIR="$HOME/Desktop/BattleCityStats"
```

Luego reiniciar terminal o ejecutar:
```bash
source ~/.zshrc
```

## 📊 Gráficos Generados

### 1️⃣ Disparos por Equipo (`1_disparos_por_equipo.png`)

Muestra cuántas balas disparó cada equipo en cada partida:

- **Eje X**: Número de partida
- **Eje Y**: Cantidad de disparos
- **Barras azules**: Equipo AZUL
- **Barras rojas**: Equipo ROJO

**Interpretación**: Identifica qué equipo es más agresivo en cada partida.

### 2️⃣ Victorias por Equipo (`2_victorias_por_equipo.png`)

Muestra el total acumulado de victorias:

- **Barras con valores absolutos y porcentajes**
- **Identifica el equipo dominante**

**Interpretación**: ¿Qué equipo es el más exitoso históricamente?

### 3️⃣ Muertes por Jugador (`3_muertes_por_jugador.png`)

Ranking de jugadores según eliminaciones recibidas:

- **Eje X**: Jugadores (Jugador 1, 2, 3, 4)
- **Eje Y**: Cantidad de muertes
- **Borde dorado**: Jugador que más murió 💀

**Interpretación**: ¿Quién necesita mejorar su estrategia defensiva?

### 4️⃣ Duración de Partidas (`4_duracion_partidas.png`)

Tiempo de cada partida y promedio:

- **Línea morada**: Duración de cada partida
- **Línea roja punteada**: Promedio general
- **Área sombreada**: Visualización de tendencia

**Interpretación**: ¿Cuánto duran típicamente las partidas?

## 📝 Formato de Logs

El sistema analiza logs con el formato:

```log
[HH:MM:SS] INFO - GAME_START|team_blue=2|team_red=2|total_players=4
[HH:MM:SS] INFO - SHOT_FIRED|player_id=0|team=0|x=100|y=200
[HH:MM:SS] WARNING - PLAYER_KILLED|killer_id=1|killer_team=0|victim_id=2|victim_team=1
[HH:MM:SS] CRITICAL - GAME_END|winner_team=0|team_name=AZUL
```

Estos logs se generan automáticamente cuando juegas.

## 🎮 Flujo de Trabajo Completo

```bash
# 1. Jugar varias partidas
python3 game_multiplayer.py
# (Los logs se guardan automáticamente en logs/)

# 2. Generar estadísticas
python3 game_stats.py

# 3. Ver gráficos
open stats_output/*.png
# o en Linux:
xdg-open stats_output/*.png
```

## 🔧 Requisitos

```bash
pip install -r requirements.txt
```

Dependencias:
- `pygame==2.6.1` - Motor del juego
- `matplotlib>=3.7.0` - Generación de gráficos

## 📈 Ejemplo de Salida

```
============================================================
🎮 GENERADOR DE ESTADÍSTICAS - BATTLE CITY MULTIPLAYER
============================================================
📁 Carpeta de salida: stats_output
   (Configurable con variable de entorno GAME_STATS_OUTPUT_DIR)
============================================================
📂 Analizando 5 archivos de log...
✓ Análisis completo: 3 partidas encontradas
  - Total disparos: 147
  - Total muertes: 12
  - Victorias equipo AZUL: 2
  - Victorias equipo ROJO: 1

============================================================
📈 RESUMEN DE ESTADÍSTICAS
============================================================

🎮 Total de partidas analizadas: 3

💥 Disparos:
   - Total: 147
   - Equipo AZUL: 78 (53.1%)
   - Equipo ROJO: 69 (46.9%)

🏆 Victorias:
   - Equipo AZUL: 2 (66.7%)
   - Equipo ROJO: 1 (33.3%)

💀 Muertes:
   - Total de eliminaciones: 12
   - Jugador 1: 2 muertes
   - Jugador 2: 3 muertes
   - Jugador 3: 4 muertes
   - Jugador 4: 3 muertes

   ⚠️  Jugador que más murió: Jugador 3 (4 veces)

⏱️  Duración de partidas:
   - Promedio: 5.23 minutos
   - Más corta: 3.50 minutos
   - Más larga: 7.12 minutos

============================================================

📊 Generando gráficos en 'stats_output/'...
  ✓ Gráfico 1 guardado: stats_output/1_disparos_por_equipo.png
  ✓ Gráfico 2 guardado: stats_output/2_victorias_por_equipo.png
  ✓ Gráfico 3 guardado: stats_output/3_muertes_por_jugador.png
  ✓ Gráfico 4 guardado: stats_output/4_duracion_partidas.png

✅ ¡Proceso completado!
```

## 🎯 Casos de Uso

### Análisis de Equipo

```bash
# Después de un torneo
python3 game_stats.py
# Responde: ¿Qué equipo es más fuerte?
```

### Análisis de Jugador

```bash
# Ver quién necesita entrenamiento
python3 game_stats.py
# Revisa el gráfico 3: Muertes por jugador
```

### Análisis de Sesión

```bash
# Comparar diferentes sesiones de juego
GAME_STATS_OUTPUT_DIR=~/Desktop/sesion_manana python3 game_stats.py
GAME_STATS_OUTPUT_DIR=~/Desktop/sesion_tarde python3 game_stats.py
```

## 🐛 Troubleshooting

### Error: "No hay archivos de log"

**Solución**: Juega al menos una partida completa para generar logs.

```bash
python3 game_multiplayer.py
# Jugar hasta que haya un ganador
```

### Error: "matplotlib no encontrado"

**Solución**: Instalar dependencias.

```bash
pip install matplotlib
```

### Los gráficos no se generan

**Solución**: Verificar permisos de escritura en la carpeta de salida.

```bash
ls -la stats_output/
chmod 755 stats_output/
```

### Los logs no tienen datos estructurados

**Solución**: Asegúrate de usar la versión actualizada de `network.py` que incluye logs estructurados (`GAME_START|`, `SHOT_FIRED|`, etc.)

## 📚 Información Técnica

### Estructura del Parser

```python
# Eventos parseados:
- GAME_START|team_blue=N|team_red=M     # Inicio de partida
- SHOT_FIRED|player_id=N|team=M         # Disparo
- PLAYER_KILLED|killer_id=N|victim_id=M # Muerte
- GAME_END|winner_team=N                # Victoria
```

### Formato de Salida

- **Resolución**: 300 DPI (alta calidad para impresión)
- **Formato**: PNG con transparencia
- **Estilo**: seaborn-v0_8-darkgrid
- **Colores**: 
  - Equipo AZUL: `#3498db`
  - Equipo ROJO: `#e74c3c`

### Variables de Entorno

| Variable | Valor por Defecto | Descripción |
|----------|-------------------|-------------|
| `GAME_STATS_OUTPUT_DIR` | `stats_output` | Carpeta donde se guardan los gráficos |

## 💡 Sugerencias

1. **Ejecuta después de cada sesión de juego** para mantener estadísticas actualizadas
2. **Usa carpetas diferentes** para distintos torneos o eventos
3. **Comparte los gráficos** exportándolos como imágenes
4. **Analiza tendencias** comparando gráficos de diferentes fechas

## 📞 Soporte

Para más información sobre el juego, consulta:
- `README_MULTIPLAYER.md` - Arquitectura del modo multijugador
- `MAPS_README.md` - Sistema de mapas configurables
- `JUEGO_FUNCIONAL.md` - Funcionalidades del juego

---

¡Ahora tienes análisis profesional de tus partidas! 🎮📊
