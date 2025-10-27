#!/bin/bash

echo "======================================"
echo "Test del juego Battle City Multiplayer"
echo "======================================"
echo ""

# Verificar que pygame está instalado
echo "[1/3] Verificando instalación de pygame..."
if /Users/francisco/CLionProjects/OS/.venv/bin/python -c "import pygame" 2>/dev/null; then
    echo "✓ Pygame instalado correctamente"
else
    echo "✗ Pygame no encontrado"
    exit 1
fi

# Verificar que los módulos se importan
echo ""
echo "[2/3] Verificando módulos del juego..."
if /Users/francisco/CLionProjects/OS/.venv/bin/python -c "import sys; sys.path.insert(0, 'Juego'); from network import NetworkManager, NetworkRole, GameState; from game_multiplayer import Tank, Direction" 2>/dev/null; then
    echo "✓ Módulos del juego importados correctamente"
else
    echo "✗ Error al importar módulos"
    exit 1
fi

# Verificar compilación del ejecutable
echo ""
echo "[3/3] Verificando ejecutable del SO..."
if [ -f "build/OS" ]; then
    echo "✓ Ejecutable del SO compilado"
else
    echo "✗ Ejecutable no encontrado"
    exit 1
fi

echo ""
echo "======================================"
echo "✓ TODOS LOS TESTS PASARON"
echo "======================================"
echo ""
echo "Para jugar:"
echo "1. Ejecuta: ./build/OS"
echo "2. Inicia sesión"
echo "3. Selecciona opción 9: Battle City Multiplayer"
echo ""
echo "Para jugar directamente sin el SO:"
echo "  /Users/francisco/CLionProjects/OS/.venv/bin/python Juego/game_multiplayer.py"
echo ""
