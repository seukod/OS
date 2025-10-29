# Battle City Multiplayer - Sistema P2P

## 📖 Descripción

Sistema de juego multiplayer Battle City integrado al sistema operativo. Soporta hasta **4 jugadores simultáneos** usando arquitectura **P2P (Peer-to-Peer)** con sockets.

## 🎮 Características

- ✅ **Hasta 4 jugadores simultáneos**
- ✅ **Arquitectura P2P**: Un jugador hostea, otros se conectan
- ✅ **El host puede jugar**: No necesita solo observar
- ✅ **Sincronización en tiempo real**: Posiciones, disparos y colisiones
- ✅ **Lobby con sistema de "Ready"**: Los jugadores marcan cuando están listos
- ✅ **Gráficos mejorados**: Colores diferentes por jugador

## 🚀 Cómo usar

### Desde el menú del sistema operativo

1. Ejecuta tu sistema operativo (ejecutable `OS`)
2. Inicia sesión con tu usuario
3. Selecciona la **opción 9: Battle City Multiplayer**
4. El juego de Python se iniciará automáticamente

### Ejecutar directamente (para testing)

```bash
cd Juego
python game_multiplayer.py
```

## 🎯 Flujo del juego

### 1. Menú Principal
Al iniciar verás 3 opciones:
- **1. HOSTEAR PARTIDA**: Crear una nueva partida (servidor)
- **2. UNIRSE A PARTIDA**: Conectarse a una partida existente (cliente)
- **3. SALIR**: Cerrar el juego

### 2. Hostear una partida
1. Selecciona "HOSTEAR PARTIDA"
2. El servidor se iniciará en `localhost:5555`
3. Tu IP se mostrará en consola
4. Espera a que otros jugadores se conecten
5. Eres el **Jugador 1 (verde)**

### 3. Unirse a una partida
1. Selecciona "UNIRSE A PARTIDA"
2. Ingresa la IP del host (deja vacío para `localhost`)
3. Presiona Enter para conectar
4. Serás asignado como **Jugador 2, 3 o 4**

### 4. Lobby (Sala de espera)
- Todos los jugadores ven "Esperando jugadores..."
- Presiona **R** cuando estés listo
- El juego inicia cuando todos presionen R

### 5. Juego
- **Flechas**: Mover tu tanque
- **Espacio**: Disparar
- Objetivo: Destruir tanques enemigos y proteger la base

## 🌐 Configuración de red

### Para jugar en red local (LAN)

**Host:**
1. Encuentra tu IP local:
   - Windows: `ipconfig` → busca "IPv4"
   - Linux/Mac: `ifconfig` o `ip addr`
2. Comparte tu IP con los otros jugadores
3. Asegúrate de que el puerto **5555** esté abierto en el firewall

**Clientes:**
1. Usa la IP del host para conectarte
2. Ejemplo: `192.168.1.100`

### Para jugar por Internet

Necesitarás:
1. **Port forwarding** en el router del host (puerto 5555)
2. La **IP pública** del host
3. O usar un servicio como **ngrok** o **Hamachi**

## 🏗️ Arquitectura técnica

### Componentes

```
OS (C++)
  └─> menu_juego.cpp
       └─> Ejecuta Python subprocess
            └─> game_multiplayer.py
                 ├─> network.py (Networking P2P)
                 └─> Pygame (Gráficos)
```

### Sistema de networking (`network.py`)

- **NetworkManager**: Gestor de conexiones
- **GameState**: Estado compartido del juego
- **Serialización**: Usa `pickle` para enviar objetos Python
- **Threading**: Hilos para recibir/enviar datos sin bloquear

### Protocolo de mensajes

```python
# Cliente → Host
{"type": "update", "player_data": {...}}
{"type": "shoot", "bullet": {...}}
{"type": "ready"}

# Host → Clientes
{"type": "state", "state": {...}}
{"type": "assign_id", "player_id": 0-3}
```

## 🎨 Colores de jugadores

- **Jugador 1 (Host)**: Verde bosque
- **Jugador 2**: Azul dodger
- **Jugador 3**: Naranja oscuro
- **Jugador 4**: Orquídea (rosa/púrpura)

## 🔧 Requisitos

### Python
- Python 3.7+
- pygame

```bash
pip install pygame
```

### C++ (para compilar el OS)
- CMake 3.20+
- Compilador C++20

## 🐛 Troubleshooting

### "No se pudo iniciar el host"
- Verifica que el puerto 5555 no esté en uso
- Revisa el firewall
- Intenta cambiar el puerto en el código

### "No se pudo conectar al host"
- Verifica la IP del host
- Asegúrate de que ambos estén en la misma red
- Revisa el firewall

### "El juego no se inicia desde C++"
- Verifica que Python esté en el PATH
- Asegúrate de que pygame esté instalado
- Revisa las rutas en `menu_juego.cpp`

### Lag o desincronización
- El host procesa todas las colisiones
- Verifica la latencia de red
- Juega en LAN para mejor rendimiento

## 📝 Notas técnicas

### ¿Por qué P2P y no Cliente-Servidor puro?

En este diseño:
- El **host actúa como servidor Y jugador**
- Los clientes se conectan al host
- El host procesa colisiones (authoritative server)
- Los clientes envían inputs y reciben estado

Esto es "pseudo-P2P" o "host-based multiplayer", común en juegos casuales.

### Sincronización

- **60 FPS** localmente
- **Actualizaciones de red**: Cada frame para el jugador local
- **Host broadcast**: Después de procesar colisiones
- **Clientes**: Interpolan posiciones recibidas

### Limitaciones actuales

- ⚠️ Máximo 4 jugadores
- ⚠️ Si el host se desconecta, la partida termina
- ⚠️ No hay reconexión automática
- ⚠️ No hay sistema de matchmaking

### Mejoras futuras

- [ ] Sistema de latencia compensada
- [ ] Predicción del lado del cliente
- [ ] Reconexión automática
- [ ] Chat de texto
- [ ] Múltiples mapas
- [ ] Power-ups y mejoras

## 📄 Licencia

Este proyecto es parte del sistema operativo OS.

## 👥 Créditos

Desarrollado como parte del curso de Sistemas Operativos.
Basado en el clásico Battle City (1985, Namco).
