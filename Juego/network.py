"""
Sistema de networking P2P para Battle City Multiplayer
Permite que un jugador hostee la partida y otros 3 se conecten
"""
import socket
import threading
import pickle
import time
from enum import Enum

def get_local_ip():
    """Obtiene la IP local de la máquina"""
    try:
        # Crear un socket temporal para obtener la IP local
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        # No necesita conectarse realmente, solo intenta conectar a una IP externa
        s.connect(("8.8.8.8", 80))
        local_ip = s.getsockname()[0]
        s.close()
        return local_ip
    except Exception:
        return "localhost"

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
            self.socket.bind(("0.0.0.0", self.port))  # Escuchar en todas las interfaces
            self.socket.listen(3)  # Máximo 3 clientes adicionales (4 jugadores total)
            self.running = True
            self.player_id = 0  # El host es el jugador 0
            
            # Obtener la IP local real
            local_ip = get_local_ip()
            
            print(f"\n{'='*50}")
            print(f"[HOST] ✓ Servidor iniciado exitosamente")
            print(f"{'='*50}")
            print(f"[HOST] Puerto: {self.port}")
            print(f"[HOST] Tu IP local es: {local_ip}")
            print(f"{'='*50}")
            print(f"[HOST] Comparte esta IP con otros jugadores:")
            print(f"       >>> {local_ip} <<<")
            print(f"{'='*50}")
            print(f"[HOST] Esperando jugadores...\n")
            
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
            print(f"\n[CLIENT] Intentando conectar a {self.host}:{self.port}...")
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket.settimeout(10)  # Timeout solo para la conexión inicial
            self.socket.connect((self.host, self.port))
            self.socket.settimeout(None)  # Sin timeout después de conectar
            self.running = True
            
            # Recibir ID del servidor
            data = self._receive_data(self.socket)
            if data and data["type"] == "assign_id":
                self.player_id = data["player_id"]
                print(f"\n{'='*50}")
                print(f"[CLIENT] ✓ Conectado al servidor exitosamente!")
                print(f"{'='*50}")
                print(f"[CLIENT] Eres el Jugador {self.player_id + 1}")
                print(f"[CLIENT] Host: {self.host}:{self.port}")
                print(f"{'='*50}\n")
            
            # Thread para recibir actualizaciones
            receive_thread = threading.Thread(target=self._receive_updates_client, daemon=True)
            receive_thread.start()
            
            return True
        except Exception as e:
            print(f"\n{'='*50}")
            print(f"[ERROR] No se pudo conectar al host")
            print(f"{'='*50}")
            print(f"Detalles: {e}")
            print(f"Verifica:")
            print(f"  - La IP del host es correcta: {self.host}")
            print(f"  - El host ya inició la partida")
            print(f"  - Ambas PCs están en la misma red")
            print(f"{'='*50}\n")
            return False
    
    def _accept_connections(self):
        """Aceptar conexiones de clientes (solo host)"""
        while self.running and len(self.clients) < 3:
            try:
                client_socket, address = self.socket.accept()
                # Sin timeout para evitar desconexiones
                client_socket.settimeout(None)
                
                player_id = len(self.clients) + 1  # IDs: 1, 2, 3
                self.clients.append({"socket": client_socket, "id": player_id, "address": address})
                
                # Enviar ID al cliente
                self._send_data(client_socket, {"type": "assign_id", "player_id": player_id})
                
                print(f"[HOST] ✓ Jugador {player_id + 1} conectado desde {address[0]}:{address[1]}")
                print(f"[HOST] Jugadores conectados: {len(self.clients) + 1}/4")
                
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
        
        # Cliente desconectado - limpiar recursos
        print(f"[HOST] Jugador {player_id + 1} desconectado")
        with self.lock:
            # Remover de la lista de clientes
            self.clients = [c for c in self.clients if c["id"] != player_id]
            # Remover del estado del juego
            if player_id in self.game_state.players:
                del self.game_state.players[player_id]
            # Remover de la lista de ready
            self.game_state.players_ready.discard(player_id)
        
        # Cerrar socket de forma segura
        try:
            client_socket.shutdown(socket.SHUT_RDWR)
        except:
            pass
        try:
            client_socket.close()
        except:
            pass
    
    def _receive_updates_client(self):
        """Recibir actualizaciones del host (solo cliente)"""
        while self.running:
            try:
                data = self._receive_data(self.socket)
                if not data:
                    print("[CLIENT] ⚠️  Host cerró la conexión")
                    break
                
                if data["type"] == "state":
                    with self.lock:
                        self.game_state = GameState.from_dict(data["state"])
                    
                    # Llamar callback si existe
                    if self.receive_callback:
                        self.receive_callback(self.game_state)
                        
            except socket.timeout:
                print("[CLIENT] ⏱️  Timeout esperando datos del host")
                continue
            except Exception as e:
                print(f"[CLIENT] ❌ Error recibiendo actualizaciones: {e}")
                break
        
        print("[CLIENT] Desconectado del host")
    
    def _broadcast_state(self):
        """Enviar estado del juego a todos los clientes (solo host)"""
        state_data = {
            "type": "state",
            "state": self.game_state.to_dict()
        }
        
        # Lista de clientes a remover (sockets cerrados)
        clients_to_remove = []
        
        for client in self.clients[:]:  # Copiar la lista para iterar de forma segura
            try:
                # Verificar si el socket está abierto
                if client["socket"].fileno() == -1:
                    clients_to_remove.append(client["id"])
                    continue
                
                self._send_data(client["socket"], state_data)
            except (BrokenPipeError, OSError, Exception) as e:
                print(f"[HOST] Cliente {client['id'] + 1} desconectado (error: {e})")
                clients_to_remove.append(client["id"])
        
        # Remover clientes desconectados
        if clients_to_remove:
            with self.lock:
                self.clients = [c for c in self.clients if c["id"] not in clients_to_remove]
                for client_id in clients_to_remove:
                    if client_id in self.game_state.players:
                        del self.game_state.players[client_id]
                    self.game_state.players_ready.discard(client_id)
    
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
            # Verificar si el socket está abierto
            if sock.fileno() == -1:
                return False
                
            serialized = pickle.dumps(data)
            # Enviar tamaño primero
            size = len(serialized)
            sock.sendall(size.to_bytes(4, byteorder='big'))
            # Enviar datos
            sock.sendall(serialized)
            return True
        except (BrokenPipeError, OSError):
            # Socket cerrado, no es un error crítico
            return False
        except Exception as e:
            raise e
    
    def _receive_data(self, sock):
        """Recibir datos serializados del socket"""
        try:
            # Recibir tamaño
            size_data = sock.recv(4)
            if not size_data or len(size_data) < 4:
                return None
            size = int.from_bytes(size_data, byteorder='big')
            
            # Validar tamaño razonable (máximo 10MB)
            if size > 10 * 1024 * 1024:
                print(f"[ERROR] Tamaño de mensaje inválido: {size} bytes")
                return None
            
            # Recibir datos
            data = b""
            while len(data) < size:
                chunk = sock.recv(min(size - len(data), 4096))
                if not chunk:
                    return None
                data += chunk
            
            return pickle.loads(data)
        except socket.timeout:
            # Timeout es normal, no es un error
            raise
        except (ConnectionResetError, BrokenPipeError, OSError):
            # Conexión cerrada
            return None
        except Exception as e:
            print(f"[ERROR] Error en _receive_data: {e}")
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
