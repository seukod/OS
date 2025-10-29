#!/usr/bin/env python3
"""
Servidor dedicado para Battle City Multiplayer
Ejecuta este script para tener un servidor puro sin jugar
"""

import sys
sys.path.insert(0, '/Users/francisco/CLionProjects/OS')

from Juego.network import DedicatedServer

if __name__ == "__main__":
    print("=" * 60)
    print("  Battle City - Servidor Dedicado")
    print("=" * 60)
    print()
    
    # Crear servidor
    server = DedicatedServer(port=5555, max_players=4)
    
    # Iniciar servidor
    if server.start():
        print("\n[INFO] Servidor corriendo...")
        print("[INFO] Los jugadores pueden conectarse ahora")
        print("[INFO] Presiona Ctrl+C para detener el servidor\n")
        
        try:
            server.run_forever()
        except KeyboardInterrupt:
            print("\n[INFO] Deteniendo servidor...")
            server.shutdown()
            print("[INFO] Servidor detenido correctamente")
    else:
        print("[ERROR] No se pudo iniciar el servidor")
        sys.exit(1)
