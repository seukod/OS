# 🎮 Battle City Multiplayer - Arquitectura Cliente-Servidor

## 📝 Cambios Importantes

El juego ahora usa una arquitectura **Cliente-Servidor dedicada**:

- **ANTES (P2P)**: El host jugaba Y manejaba la red al mismo tiempo
- **AHORA (Cliente-Servidor)**: Servidor dedicado + Todos son clientes

## 🏗️ Nueva Arquitectura

### **Servidor Dedicado** 
- Corre en un hilo separado
- NO juega, solo maneja el estado del juego
- Procesa colisiones, balas, y sincronización
- Puede correr en segundo plano

### **Clientes (Jugadores)**
- TODOS son clientes (incluido quien inicia el servidor)
- Solo envían inputs y reciben estado
- Renderizado local basado en el estado del servidor

## 🚀 Cómo Jugar

### **Opción 1: Iniciar Servidor y Jugar (Recomendado)**

1. Ejecuta el juego:
   ```bash
   /Users/francisco/CLionProjects/OS/.venv/bin/python Juego/game_multiplayer.py
   ```

2. Selecciona: **"1. INICIAR SERVIDOR Y JUGAR"**
   - El servidor se inicia automáticamente en segundo plano
   - Te conectas como jugador automáticamente
   - Comparte tu IP con otros jugadores

3. Otros jugadores:
   - Ejecutan el juego
   - Seleccionan: **"2. CONECTARSE A SERVIDOR"**
   - Ingresan tu IP

### **Opción 2: Servidor Dedicado Separado**

Si quieres un servidor que NO juegue (solo administra):

1. **Terminal 1 (Servidor)**:
   ```python
   from network import DedicatedServer
   server = DedicatedServer()
   server.start()
   server.run_forever()
   ```

2. **Terminals 2-5 (Jugadores)**:
   ```bash
   python game_multiplayer.py
   # Selecciona "2. CONECTARSE A SERVIDOR"
   ```

## 🎯 Ventajas de la Nueva Arquitectura

✅ **Más estable**: El servidor no se ve afectado por lag del jugador  
✅ **Mejor sincronización**: Una sola fuente de verdad (el servidor)  
✅ **Escalable**: Fácil cambiar de 4 a más jugadores  
✅ **Sin ventaja injusta**: El host no tiene ventaja al procesar primero  
✅ **Servidor dedicado**: Puede correr en una máquina separada  

## 📊 Flujo de Datos

```
[Jugador 1] ─────┐
                  │
[Jugador 2] ─────┤
                  ├──> [SERVIDOR] ──> Procesa colisiones
[Jugador 3] ─────┤      (dedicado)     y estado del juego
                  │                           │
[Jugador 4] ─────┘                           │
                                              │
        ┌─────────────────────────────────────┘
        │
        └──> Broadcast del estado a todos los clientes
```

## 🔧 Cambios Técnicos

### **network.py**
- Nueva clase: `DedicatedServer`
- `NetworkManager` ahora es solo cliente
- Eliminado: `NetworkRole.HOST` (ya no existe)

### **game_multiplayer.py**
- El cliente NO procesa colisiones
- Solo renderiza basado en el estado recibido
- Nuevo menú con opciones de servidor/cliente
- Soporte para iniciar servidor en segundo plano

## 🎮 Controles (Sin Cambios)

- **Flechas**: Mover tanque
- **Espacio**: Disparar
- **R**: Marcar como listo (en lobby)

## 🌐 Para Jugar en Red

### **En la PC que inicia el servidor:**
```bash
# Obtener IP
ifconfig | grep "inet " | grep -v 127.0.0.1

# Iniciar juego y seleccionar opción 1
python game_multiplayer.py
```

### **En otras PCs:**
```bash
# Iniciar juego y seleccionar opción 2
python game_multiplayer.py
# Ingresar la IP del servidor cuando se solicite
```

## ⚙️ Configuración Avanzada

### Cambiar puerto del servidor:

```python
# En network.py o al iniciar:
server = DedicatedServer(port=6000, max_players=4)
```

### Servidor dedicado puro (sin UI):

```python
# server_only.py
from network import DedicatedServer

if __name__ == "__main__":
    server = DedicatedServer(port=5555, max_players=4)
    if server.start():
        print("Servidor iniciado. Presiona Ctrl+C para detener.")
        server.run_forever()
```

## 📋 Requisitos

- Python 3.7+
- Pygame 2.x
- Mismo puerto abierto en firewall (5555)

## 🐛 Solución de Problemas

### "No se pudo iniciar el servidor"
- El puerto 5555 está en uso: `lsof -i :5555`
- Cambia el puerto o cierra el proceso

### "No se pudo conectar al servidor"
- Verifica que el servidor esté corriendo
- Confirma la IP correcta
- Ambas PCs en la misma red

### El juego se congela
- Verifica la conexión de red
- Revisa la consola para mensajes de error

## ✨ Próximas Mejoras Posibles

- [ ] Reconexión automática
- [ ] Chat entre jugadores
- [ ] Múltiples mapas
- [ ] Power-ups
- [ ] Sistema de ranked/matchmaking
- [ ] Replays y estadísticas

---

**¡Disfruta el juego con la nueva arquitectura más robusta! 🚀**
