"""
Sistema de networking P2P para Battle City Multiplayer
Permite que un jugador hostee la partida y otros 3 se conecten
"""
import socket
import threading
import pickle
import time
from enum import Enum

class NetworkRole(Enum):
    HOST = "host"
    CLIENT = "client"

class GameState:
    """Estado del juego compartido entre todos los jugadores"""
    def __init__(self):
        self.players = {}  # {player_id: {"x": x, "y": y, "direction": dir, "lives": lives, "is_alive": bool}}
        self.bullets = []  # Lista de balas activas
        self.walls = []  # Estado de los muros
        self.base_destroyed = False
        self.game_started = False
        self.players_ready = set()
    
    def to_dict(self):
        return {
            "players": self.players,
            "bullets": self.bullets,
            "walls": self.walls,
            "base_destroyed": self.base_destroyed,
            "game_started": self.game_started,
            "players_ready": list(self.players_ready)
        }
    
    @staticmethod
    def from_dict(data):
        state = GameState()
        state.players = data.get("players", {})
        state.bullets = data.get("bullets", [])
        state.walls = data.get("walls", [])
        state.base_destroyed = data.get("base_destroyed", False)
        state.game_started = data.get("game_started", False)
        state.players_ready = set(data.get("players_ready", []))
        return state

class NetworkManager:
    """Gestor de red P2P para el juego"""
    def __init__(self, role, host="localhost", port=5555):
        self.role = role
        self.host = host
        self.port = port
        self.socket = None
        self.clients = []  # Solo para el host
        self.running = False
        self.game_state = GameState()
        self.player_id = None
        self.lock = threading.Lock()
        self.receive_callback = None
        
    def start_host(self):
        """Iniciar como host (servidor P2P)"""
        try:
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            self.socket.bind((self.host, self.port))
            self.socket.listen(3)  # Máximo 3 clientes adicionales (4 jugadores total)
            self.running = True
            self.player_id = 0  # El host es el jugador 0
            
            print(f"[HOST] Servidor iniciado en {self.host}:{self.port}")
            print(f"[HOST] Esperando jugadores...")
            
            # Thread para aceptar conexiones
            accept_thread = threading.Thread(target=self._accept_connections, daemon=True)
            accept_thread.start()
            
            return True
        except Exception as e:
            print(f"[ERROR] No se pudo iniciar el host: {e}")
            return False
    
    def start_client(self):
        """Iniciar como cliente"""
        try:
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket.connect((self.host, self.port))
            self.running = True
            
            # Recibir ID del servidor
            data = self._receive_data(self.socket)
            if data and data["type"] == "assign_id":
                self.player_id = data["player_id"]
                print(f"[CLIENT] Conectado al host. Tu ID es: {self.player_id}")
            
            # Thread para recibir actualizaciones
            receive_thread = threading.Thread(target=self._receive_updates_client, daemon=True)
            receive_thread.start()
            
            return True
        except Exception as e:
            print(f"[ERROR] No se pudo conectar al host: {e}")
            return False
    
    def _accept_connections(self):
        """Aceptar conexiones de clientes (solo host)"""
        while self.running and len(self.clients) < 3:
            try:
                client_socket, address = self.socket.accept()
                player_id = len(self.clients) + 1  # IDs: 1, 2, 3
                self.clients.append({"socket": client_socket, "id": player_id, "address": address})
                
                # Enviar ID al cliente
                self._send_data(client_socket, {"type": "assign_id", "player_id": player_id})
                
                print(f"[HOST] Jugador {player_id} conectado desde {address}")
                
                # Thread para manejar este cliente
                client_thread = threading.Thread(
                    target=self._handle_client,
                    args=(client_socket, player_id),
                    daemon=True
                )
                client_thread.start()
                
            except Exception as e:
                if self.running:
                    print(f"[ERROR] Error aceptando conexión: {e}")
    
    def _handle_client(self, client_socket, player_id):
        """Manejar mensajes de un cliente (solo host)"""
        while self.running:
            try:
                data = self._receive_data(client_socket)
                if not data:
                    break
                
                # Procesar el mensaje del cliente
                if data["type"] == "update":
                    with self.lock:
                        # Actualizar estado del jugador
                        self.game_state.players[player_id] = data["player_data"]
                        
                        # Propagar a todos los otros clientes
                        self._broadcast_state()
                
                elif data["type"] == "shoot":
                    with self.lock:
                        self.game_state.bullets.append(data["bullet"])
                        self._broadcast_state()
                
                elif data["type"] == "ready":
                    with self.lock:
                        self.game_state.players_ready.add(player_id)
                        if len(self.game_state.players_ready) == len(self.clients) + 1:
                            self.game_state.game_started = True
                        self._broadcast_state()
                        
            except Exception as e:
                print(f"[ERROR] Error manejando cliente {player_id}: {e}")
                break
        
        # Cliente desconectado
        print(f"[HOST] Jugador {player_id} desconectado")
        with self.lock:
            if player_id in self.game_state.players:
                del self.game_state.players[player_id]
        client_socket.close()
    
    def _receive_updates_client(self):
        """Recibir actualizaciones del host (solo cliente)"""
        while self.running:
            try:
                data = self._receive_data(self.socket)
                if not data:
                    break
                
                if data["type"] == "state":
                    with self.lock:
                        self.game_state = GameState.from_dict(data["state"])
                    
                    # Llamar callback si existe
                    if self.receive_callback:
                        self.receive_callback(self.game_state)
                        
            except Exception as e:
                print(f"[ERROR] Error recibiendo actualizaciones: {e}")
                break
        
        print("[CLIENT] Desconectado del host")
    
    def _broadcast_state(self):
        """Enviar estado del juego a todos los clientes (solo host)"""
        state_data = {
            "type": "state",
            "state": self.game_state.to_dict()
        }
        
        for client in self.clients:
            try:
                self._send_data(client["socket"], state_data)
            except Exception as e:
                print(f"[ERROR] Error enviando a cliente {client['id']}: {e}")
    
    def send_update(self, player_data):
        """Enviar actualización del jugador local"""
        if self.role == NetworkRole.HOST:
            with self.lock:
                self.game_state.players[self.player_id] = player_data
                self._broadcast_state()
        else:
            # Cliente envía al host
            try:
                self._send_data(self.socket, {"type": "update", "player_data": player_data})
            except Exception as e:
                print(f"[ERROR] Error enviando actualización: {e}")
    
    def send_shoot(self, bullet_data):
        """Enviar evento de disparo"""
        if self.role == NetworkRole.HOST:
            with self.lock:
                self.game_state.bullets.append(bullet_data)
                self._broadcast_state()
        else:
            try:
                self._send_data(self.socket, {"type": "shoot", "bullet": bullet_data})
            except Exception as e:
                print(f"[ERROR] Error enviando disparo: {e}")
    
    def send_ready(self):
        """Indicar que el jugador está listo"""
        if self.role == NetworkRole.HOST:
            with self.lock:
                self.game_state.players_ready.add(self.player_id)
                if len(self.game_state.players_ready) == len(self.clients) + 1:
                    self.game_state.game_started = True
                self._broadcast_state()
        else:
            try:
                self._send_data(self.socket, {"type": "ready"})
            except Exception as e:
                print(f"[ERROR] Error enviando ready: {e}")
    
    def _send_data(self, sock, data):
        """Enviar datos serializados por socket"""
        try:
            serialized = pickle.dumps(data)
            # Enviar tamaño primero
            size = len(serialized)
            sock.sendall(size.to_bytes(4, byteorder='big'))
            # Enviar datos
            sock.sendall(serialized)
        except Exception as e:
            raise e
    
    def _receive_data(self, sock):
        """Recibir datos serializados del socket"""
        try:
            # Recibir tamaño
            size_data = sock.recv(4)
            if not size_data:
                return None
            size = int.from_bytes(size_data, byteorder='big')
            
            # Recibir datos
            data = b""
            while len(data) < size:
                chunk = sock.recv(min(size - len(data), 4096))
                if not chunk:
                    return None
                data += chunk
            
            return pickle.loads(data)
        except Exception as e:
            return None
    
    def get_game_state(self):
        """Obtener el estado actual del juego"""
        with self.lock:
            return self.game_state
    
    def set_receive_callback(self, callback):
        """Establecer callback para cuando se reciben actualizaciones"""
        self.receive_callback = callback
    
    def shutdown(self):
        """Cerrar conexiones"""
        self.running = False
        
        if self.role == NetworkRole.HOST:
            for client in self.clients:
                try:
                    client["socket"].close()
                except:
                    pass
        
        if self.socket:
            try:
                self.socket.close()
            except:
                pass
        
        print("[NETWORK] Conexión cerrada")
