"""
Script de prueba simple para verificar el networking
"""
import sys
sys.path.append('.')

from network import NetworkManager, NetworkRole
import time

def test_host():
    print("=== TEST: HOST ===")
    network = NetworkManager(NetworkRole.HOST, port=5556)  # Puerto diferente para testing
    if network.start_host():
        print("[OK] Host iniciado correctamente")
        print("Esperando 5 segundos...")
        time.sleep(5)
        print("Cerrando host...")
        network.shutdown()
        print("[OK] Test completado")
        return True
    else:
        print("[ERROR] No se pudo iniciar el host")
        return False

def test_client():
    print("=== TEST: CLIENT ===")
    network = NetworkManager(NetworkRole.CLIENT, host="localhost", port=5556)
    if network.start_client():
        print("[OK] Cliente conectado correctamente")
        print("Esperando 3 segundos...")
        time.sleep(3)
        print("Cerrando cliente...")
        network.shutdown()
        print("[OK] Test completado")
        return True
    else:
        print("[ERROR] No se pudo conectar al host")
        return False

if __name__ == "__main__":
    print("Test de networking para Battle City Multiplayer")
    print("=" * 50)
    print()
    
    if len(sys.argv) > 1 and sys.argv[1] == "client":
        test_client()
    else:
        test_host()
