#!/bin/bash

# Script para iniciar múltiples instancias del juego para testing local

echo "=================================================="
echo "  Battle City Multiplayer - Test Local Setup"
echo "=================================================="
echo ""
echo "Este script te ayudará a iniciar múltiples instancias"
echo "del juego para probar el modo multiplayer localmente."
echo ""
echo "Instrucciones:"
echo "1. Primera ventana: Selecciona HOSTEAR PARTIDA"
echo "2. Otras ventanas: Selecciona UNIRSE (deja IP vacía)"
echo "3. Todos presionan R cuando estén listos"
echo ""
echo "¿Cuántas instancias quieres iniciar? (2-4): "
read -r num_players

if [[ ! "$num_players" =~ ^[2-4]$ ]]; then
    echo "Error: Debes ingresar un número entre 2 y 4"
    exit 1
fi

echo ""
echo "Iniciando $num_players instancias del juego..."
echo ""

PYTHON_PATH="/Users/francisco/CLionProjects/OS/.venv/bin/python"
GAME_PATH="/Users/francisco/CLionProjects/OS/Juego/game_multiplayer.py"

for i in $(seq 1 "$num_players"); do
    echo "Iniciando Jugador $i..."
    osascript -e "tell app \"Terminal\" to do script \"cd /Users/francisco/CLionProjects/OS && $PYTHON_PATH $GAME_PATH\"" &
    sleep 1
done

echo ""
echo "✓ $num_players ventanas iniciadas"
echo ""
echo "Recuerda:"
echo "  - Primera ventana: HOSTEAR"
echo "  - Otras ventanas: UNIRSE (IP vacía = localhost)"
echo ""
