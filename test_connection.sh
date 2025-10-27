#!/bin/bash

echo "========================================"
echo "  Test de Conexión - Battle City"
echo "========================================"
echo ""

# 1. Verificar IP local
echo "1️⃣  Tu IP local:"
IP=$(ipconfig getifaddr en0 2>/dev/null || ipconfig getifaddr en1 2>/dev/null)
if [ -z "$IP" ]; then
    echo "   ❌ No se pudo detectar IP automáticamente"
    echo "   Ejecuta: ifconfig | grep 'inet '"
else
    echo "   ✅ $IP"
fi
echo ""

# 2. Verificar puerto
echo "2️⃣  Verificando puerto 5555..."
if lsof -i :5555 > /dev/null 2>&1; then
    echo "   ⚠️  Puerto 5555 está en uso"
    lsof -i :5555
    echo ""
    echo "   Para liberar el puerto:"
    echo "   kill -9 \$(lsof -t -i :5555)"
else
    echo "   ✅ Puerto 5555 disponible"
fi
echo ""

# 3. Verificar procesos Python/Pygame
echo "3️⃣  Procesos del juego activos:"
PROCS=$(ps aux | grep "game_multiplayer.py" | grep -v grep | wc -l)
if [ "$PROCS" -gt 0 ]; then
    echo "   🎮 $PROCS instancia(s) del juego ejecutándose"
    ps aux | grep "game_multiplayer.py" | grep -v grep
else
    echo "   ⚪ No hay instancias del juego ejecutándose"
fi
echo ""

# 4. Verificar conectividad local
echo "4️⃣  Test de conectividad localhost..."
if nc -z localhost 5555 2>/dev/null; then
    echo "   ✅ Servidor en localhost:5555 responde"
else
    echo "   ⚪ No hay servidor escuchando en localhost:5555"
fi
echo ""

# 5. Verificar Pygame
echo "5️⃣  Verificando Pygame..."
if /Users/francisco/CLionProjects/OS/.venv/bin/python -c "import pygame" 2>/dev/null; then
    VERSION=$(/Users/francisco/CLionProjects/OS/.venv/bin/python -c "import pygame; print(pygame.version.ver)" 2>/dev/null)
    echo "   ✅ Pygame $VERSION instalado"
else
    echo "   ❌ Pygame no instalado"
fi
echo ""

echo "========================================"
echo "  Resumen"
echo "========================================"
echo ""
echo "Para jugar en red local:"
echo "  HOST: Comparte la IP $IP"
echo "  CLIENTE: Usa esa IP para conectarse"
echo ""
echo "Para testing local:"
echo "  Usa 'localhost' o '127.0.0.1' en ambos"
echo ""
