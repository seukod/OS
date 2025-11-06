"""
Sistema de networking Cliente-Servidor para Battle City Multiplayer
Servidor dedicado que maneja hasta 4 clientes jugadores
"""
import socket
import threading
import pickle
import time
from enum import Enum

def get_local_ip():
    """Obtiene la IP local de la máquina"""
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect(("8.8.8.8", 80))
        local_ip = s.getsockname()[0]
        s.close()
        return local_ip
    except Exception:
        return "localhost"

class NetworkRole(Enum):
    SERVER = "server"  # Servidor dedicado (no juega)
    CLIENT = "client"  # Cliente jugador

class GameState:
    """Estado del juego compartido entre todos los jugadores"""
    def __init__(self):
        self.players = {}
        self.bullets = []
        self.walls = []
        self.game_started = False
        self.players_ready = set()
        self.teams = {0: [0, 1], 1: [2, 3]}  # Equipo 0: jugadores 0,1 | Equipo 1: jugadores 2,3
        self.game_over = False
        self.winning_team = None
    
    def to_dict(self):
        return {
            "players": self.players,
            "bullets": self.bullets,
            "walls": self.walls,
            "game_started": self.game_started,
            "players_ready": list(self.players_ready),
            "teams": self.teams,
            "game_over": self.game_over,
            "winning_team": self.winning_team
        }
    
    @staticmethod
    def from_dict(data):
        state = GameState()
        state.players = data.get("players", {})
        state.bullets = data.get("bullets", [])
        state.walls = data.get("walls", [])
        state.game_started = data.get("game_started", False)
        state.players_ready = set(data.get("players_ready", []))
        state.teams = data.get("teams", {0: [0, 1], 1: [2, 3]})
        state.game_over = data.get("game_over", False)
        state.winning_team = data.get("winning_team", None)
        return state
    
    def check_victory(self):
        """Verificar si algún equipo ganó"""
        # Contar jugadores vivos por equipo
        team_0_alive = sum(1 for pid in self.teams[0] if pid in self.players and self.players[pid].get("is_alive", False))
        team_1_alive = sum(1 for pid in self.teams[1] if pid in self.players and self.players[pid].get("is_alive", False))
        
        if team_0_alive == 0 and team_1_alive > 0:
            self.game_over = True
            self.winning_team = 1
            return True
        elif team_1_alive == 0 and team_0_alive > 0:
            self.game_over = True
            self.winning_team = 0
            return True
        
        return False

class DedicatedServer:
    """Servidor dedicado que NO juega, solo maneja el estado del juego"""
    def __init__(self, port=5555, max_players=4):
        self.port = port
        self.max_players = max_players
        self.socket = None
        self.clients = []
        self.running = False
        self.game_state = GameState()
        self.lock = threading.Lock()
        self.tick_rate = 60  # Actualizaciones por segundo
        self._initialize_walls()  # Inicializar muros
        
    def _initialize_walls(self):
        """Inicializar los muros del servidor (deben coincidir con los del cliente)"""
        TILE_SIZE = 32
        SCREEN_WIDTH = 800
        SCREEN_HEIGHT = 600
        
        walls = []
        # Línea horizontal superior izquierda
        for x in range(150, 350, TILE_SIZE):
            walls.append({"x": x, "y": 200, "width": TILE_SIZE, "height": TILE_SIZE})
        # Línea horizontal superior derecha
        for x in range(450, 650, TILE_SIZE):
            walls.append({"x": x, "y": 300, "width": TILE_SIZE, "height": TILE_SIZE})
        # Línea vertical izquierda
        for y in range(100, 200, TILE_SIZE):
            walls.append({"x": 100, "y": y, "width": TILE_SIZE, "height": TILE_SIZE})
        # Línea vertical derecha
        for y in range(350, 450, TILE_SIZE):
            walls.append({"x": 700, "y": y, "width": TILE_SIZE, "height": TILE_SIZE})
        # Línea horizontal inferior (centro)
        for x in range(SCREEN_WIDTH//2 - TILE_SIZE*2, SCREEN_WIDTH//2 + TILE_SIZE*2, TILE_SIZE):
            walls.append({"x": x, "y": SCREEN_HEIGHT - TILE_SIZE*3, "width": TILE_SIZE, "height": TILE_SIZE})
        
        self.game_state.walls = walls
        
    def start(self):
        """Iniciar el servidor dedicado"""
        try:
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            self.socket.bind(("0.0.0.0", self.port))
            self.socket.listen(self.max_players)
            self.running = True
            
            local_ip = get_local_ip()
            
            print(f"\n{'='*60}")
            print(f"[SERVER] ✓ Servidor dedicado iniciado")
            print(f"{'='*60}")
            print(f"[SERVER] Puerto: {self.port}")
            print(f"[SERVER] Máximo jugadores: {self.max_players}")
            print(f"[SERVER] IP para compartir: {local_ip}")
            print(f"{'='*60}")
            print(f"[SERVER] Los jugadores deben conectarse a: {local_ip}")
            print(f"{'='*60}")
            print(f"[SERVER] Esperando jugadores...\n")
            
            accept_thread = threading.Thread(target=self._accept_connections, daemon=True)
            accept_thread.start()
            
            # Iniciar el game loop del servidor
            game_loop_thread = threading.Thread(target=self._game_loop, daemon=True)
            game_loop_thread.start()
            
            return True
        except Exception as e:
            print(f"[ERROR] No se pudo iniciar el servidor: {e}")
            return False
    
    def _accept_connections(self):
        """Aceptar conexiones de clientes"""
        while self.running and len(self.clients) < self.max_players:
            try:
                client_socket, address = self.socket.accept()
                client_socket.settimeout(None)
                
                player_id = len(self.clients)
                
                client_info = {
                    "socket": client_socket,
                    "id": player_id,
                    "address": address
                }
                self.clients.append(client_info)
                
                self._send_data(client_socket, {"type": "assign_id", "player_id": player_id})
                
                print(f"[SERVER] ✓ Jugador {player_id + 1} conectado desde {address[0]}:{address[1]}")
                print(f"[SERVER] Jugadores conectados: {len(self.clients)}/{self.max_players}")
                
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
        """Manejar mensajes de un cliente"""
        while self.running:
            try:
                data = self._receive_data(client_socket)
                if not data:
                    print(f"[SERVER] ⚠️  Jugador {player_id + 1} perdió la conexión (sin datos)")
                    break
                
                if data["type"] == "update":
                    with self.lock:
                        self.game_state.players[player_id] = data["player_data"]
                        self._broadcast_state()
                
                elif data["type"] == "shoot":
                    with self.lock:
                        bullet_data = data["bullet"]
                        print(f"[SERVER] 🔫 Disparo recibido: {bullet_data}")
                        self.game_state.bullets.append(bullet_data)
                        self._broadcast_state()
                
                elif data["type"] == "ready":
                    with self.lock:
                        self.game_state.players_ready.add(player_id)
                        if len(self.game_state.players_ready) == len(self.clients):
                            self.game_state.game_started = True
                            print(f"[SERVER] 🎮 Juego iniciado! Todos los jugadores listos.")
                        self._broadcast_state()
                        
            except (ConnectionResetError, BrokenPipeError, OSError) as e:
                print(f"[SERVER] ⚠️  Jugador {player_id + 1} desconectado inesperadamente: {type(e).__name__}")
                break
            except Exception as e:
                print(f"[SERVER] ❌ Error manejando cliente {player_id + 1}: {e}")
                break
        
        # Limpieza al desconectar
        print(f"[SERVER] 🔌 Jugador {player_id + 1} desconectado - Limpiando...")
        with self.lock:
            # Remover de lista de clientes
            self.clients = [c for c in self.clients if c["id"] != player_id]
            
            # Si el jugador estaba en el juego, marcarlo como muerto
            if player_id in self.game_state.players:
                if self.game_state.game_started:
                    # Durante el juego, marcar como muerto pero mantener en el estado
                    self.game_state.players[player_id]["is_alive"] = False
                    self.game_state.players[player_id]["lives"] = 0
                    print(f"[SERVER] 💀 Jugador {player_id + 1} marcado como eliminado (desconexión)")
                    
                    # Verificar si esto causa victoria
                    if self.game_state.check_victory():
                        print(f"[SERVER] 🏆 ¡EQUIPO {self.game_state.winning_team + 1} GANA por desconexión del enemigo!")
                else:
                    # En el lobby, simplemente remover
                    del self.game_state.players[player_id]
            
            # Remover de jugadores listos
            self.game_state.players_ready.discard(player_id)
            
            # Notificar a otros jugadores
            self._broadcast_state()
        
        # Cerrar socket de forma segura
        try:
            client_socket.shutdown(socket.SHUT_RDWR)
        except:
            pass
        try:
            client_socket.close()
        except:
            pass
        
        print(f"[SERVER] ✓ Limpieza completada para Jugador {player_id + 1}")
    
    def _broadcast_state(self):
        """Enviar estado del juego a todos los clientes"""
        state_data = {
            "type": "state",
            "state": self.game_state.to_dict()
        }
        
        clients_to_remove = []
        
        for client in self.clients[:]:
            try:
                # Verificar si el socket sigue válido
                if client["socket"].fileno() == -1:
                    print(f"[SERVER] ⚠️  Socket inválido para Jugador {client['id'] + 1}")
                    clients_to_remove.append(client["id"])
                    continue
                
                self._send_data(client["socket"], state_data)
            except (BrokenPipeError, OSError) as e:
                print(f"[SERVER] ⚠️  Error enviando a Jugador {client['id'] + 1}: {type(e).__name__}")
                clients_to_remove.append(client["id"])
            except Exception as e:
                print(f"[SERVER] ❌ Error inesperado con Jugador {client['id'] + 1}: {e}")
                clients_to_remove.append(client["id"])
        
        if clients_to_remove:
            print(f"[SERVER] 🧹 Limpiando {len(clients_to_remove)} cliente(s) desconectado(s)")
            with self.lock:
                self.clients = [c for c in self.clients if c["id"] not in clients_to_remove]
                for client_id in clients_to_remove:
                    if client_id in self.game_state.players:
                        if self.game_state.game_started:
                            # Durante el juego, marcar como muerto
                            self.game_state.players[client_id]["is_alive"] = False
                            self.game_state.players[client_id]["lives"] = 0
                            print(f"[SERVER] 💀 Jugador {client_id + 1} marcado como eliminado (broadcast)")
                        else:
                            # En lobby, remover completamente
                            del self.game_state.players[client_id]
                    self.game_state.players_ready.discard(client_id)
    
    def _game_loop(self):
        """Game loop del servidor que actualiza la lógica del juego"""
        TILE_SIZE = 32
        SCREEN_WIDTH = 800
        SCREEN_HEIGHT = 600
        
        while self.running:
            if self.game_state.game_started:
                with self.lock:
                    # Debug: mostrar cantidad de balas
                    if len(self.game_state.bullets) > 0:
                        print(f"[SERVER] 🎯 Procesando {len(self.game_state.bullets)} balas")
                    
                    # Actualizar balas
                    for bullet in self.game_state.bullets[:]:
                        if not bullet.get("active", True):
                            self.game_state.bullets.remove(bullet)
                            continue
                        
                        # Actualizar posición de la bala
                        direction = bullet["direction"]
                        speed = bullet.get("speed", 5)
                        
                        if direction == 0:  # UP
                            bullet["y"] -= speed
                        elif direction == 1:  # DOWN
                            bullet["y"] += speed
                        elif direction == 2:  # LEFT
                            bullet["x"] -= speed
                        elif direction == 3:  # RIGHT
                            bullet["x"] += speed
                        
                        # Verificar si la bala salió de la pantalla
                        if (bullet["x"] < 0 or bullet["x"] > SCREEN_WIDTH or
                            bullet["y"] < 0 or bullet["y"] > SCREEN_HEIGHT):
                            bullet["active"] = False
                            continue
                        
                        # Colisiones con muros
                        bullet_rect = {
                            "x": bullet["x"],
                            "y": bullet["y"],
                            "width": bullet.get("width", 4),
                            "height": bullet.get("height", 4)
                        }
                        
                        hit_wall = False
                        for wall in self.game_state.walls:
                            if self._check_collision(bullet_rect, wall):
                                bullet["active"] = False
                                hit_wall = True
                                print(f"[SERVER] 💥 Bala impactó un muro en ({wall['x']}, {wall['y']})")
                                print(f"[SERVER]    Bala rect: {bullet_rect}")
                                print(f"[SERVER]    Muro rect: {wall}")
                                break
                        
                        if hit_wall:
                            continue
                        
                        # Colisiones con tanques
                        for player_id, player_data in self.game_state.players.items():
                            # No colisionar con el tanque que disparó
                            if player_id == bullet["owner_id"]:
                                continue
                            
                            # Solo colisionar con tanques vivos
                            if not player_data.get("is_alive", True):
                                continue
                            
                            # Verificar colisión
                            tank_rect = {
                                "x": player_data["x"],
                                "y": player_data["y"],
                                "width": TILE_SIZE,
                                "height": TILE_SIZE
                            }
                            
                            if self._check_collision(bullet_rect, tank_rect):
                                # Reducir vida del tanque
                                player_data["lives"] = player_data.get("lives", 3) - 1
                                print(f"[SERVER] 🎯 Jugador {player_id + 1} impactado! Vidas: {player_data['lives']}")
                                if player_data["lives"] <= 0:
                                    player_data["is_alive"] = False
                                    print(f"[SERVER] 💀 Jugador {player_id + 1} eliminado!")
                                
                                # Desactivar bala
                                bullet["active"] = False
                                break
                    
                    # Limpiar balas inactivas
                    active_before = len(self.game_state.bullets)
                    self.game_state.bullets = [b for b in self.game_state.bullets if b.get("active", True)]
                    active_after = len(self.game_state.bullets)
                    if active_before != active_after:
                        print(f"[SERVER] 🧹 Limpiadas {active_before - active_after} balas. Activas: {active_after}")
                    
                    # Verificar victoria
                    if self.game_state.check_victory():
                        print(f"[SERVER] 🏆 ¡EQUIPO {self.game_state.winning_team + 1} GANA!")
                    
                    # Broadcast del estado actualizado
                    self._broadcast_state()
            
            # 60 FPS
            time.sleep(1.0 / self.tick_rate)
    
    def _check_collision(self, rect1, rect2):
        """Verificar colisión entre dos rectángulos"""
        return (rect1["x"] < rect2["x"] + rect2["width"] and
                rect1["x"] + rect1["width"] > rect2["x"] and
                rect1["y"] < rect2["y"] + rect2["height"] and
                rect1["y"] + rect1["height"] > rect2["y"])
    
    def _send_data(self, sock, data):
        """Enviar datos serializados por socket"""
        try:
            if sock.fileno() == -1:
                return False
            serialized = pickle.dumps(data)
            size = len(serialized)
            sock.sendall(size.to_bytes(4, byteorder='big'))
            sock.sendall(serialized)
            return True
        except (BrokenPipeError, OSError):
            return False
        except Exception as e:
            return False
    
    def _receive_data(self, sock):
        """Recibir datos serializados del socket"""
        try:
            size_data = sock.recv(4)
            if not size_data or len(size_data) < 4:
                return None
            size = int.from_bytes(size_data, byteorder='big')
            
            if size > 10 * 1024 * 1024:
                return None
            
            data = b""
            while len(data) < size:
                chunk = sock.recv(min(size - len(data), 4096))
                if not chunk:
                    return None
                data += chunk
            
            return pickle.loads(data)
        except:
            return None
    
    def run_forever(self):
        """Mantener el servidor corriendo"""
        print(f"[SERVER] Servidor ejecutándose. Presiona Ctrl+C para detener.")
        try:
            while self.running:
                time.sleep(1)
        except KeyboardInterrupt:
            print(f"\n[SERVER] Deteniendo servidor...")
            self.shutdown()
    
    def shutdown(self):
        """Cerrar servidor"""
        self.running = False
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
        print(f"[SERVER] Servidor detenido")

class NetworkManager:
    """Cliente de red (todos los jugadores son clientes)"""
    def __init__(self, host="localhost", port=5555):
        self.host = host
        self.port = port
        self.socket = None
        self.running = False
        self.game_state = GameState()
        self.last_game_state = None  # Almacenar último estado para acceso desde UI
        self.player_id = None
        self.lock = threading.Lock()
        self.receive_callback = None
    
    def start_client(self):
        """Conectar al servidor como cliente"""
        try:
            print(f"\n[CLIENT] Intentando conectar a {self.host}:{self.port}...")
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket.settimeout(10)
            self.socket.connect((self.host, self.port))
            self.socket.settimeout(None)
            self.running = True
            
            data = self._receive_data(self.socket)
            if data and data["type"] == "assign_id":
                self.player_id = data["player_id"]
                print(f"\n{'='*50}")
                print(f"[CLIENT] ✓ Conectado al servidor exitosamente!")
                print(f"{'='*50}")
                print(f"[CLIENT] Eres el Jugador {self.player_id + 1}")
                print(f"[CLIENT] Servidor: {self.host}:{self.port}")
                print(f"{'='*50}\n")
            
            receive_thread = threading.Thread(target=self._receive_updates, daemon=True)
            receive_thread.start()
            
            return True
        except Exception as e:
            print(f"\n{'='*50}")
            print(f"[ERROR] No se pudo conectar al servidor")
            print(f"{'='*50}")
            print(f"Detalles: {e}")
            print(f"Verifica:")
            print(f"  - La IP del servidor es correcta: {self.host}")
            print(f"  - El servidor está ejecutándose")
            print(f"  - Ambas PCs están en la misma red")
            print(f"{'='*50}\n")
            return False
    
    def _receive_updates(self):
        """Recibir actualizaciones del servidor"""
        while self.running:
            try:
                data = self._receive_data(self.socket)
                if not data:
                    print("[CLIENT] ⚠️  Servidor cerró la conexión")
                    self.running = False
                    break
                
                if data["type"] == "state":
                    with self.lock:
                        self.game_state = GameState.from_dict(data["state"])
                        self.last_game_state = self.game_state  # Guardar para acceso desde UI
                    
                    if self.receive_callback:
                        self.receive_callback(self.game_state)
                        
            except (ConnectionResetError, BrokenPipeError, OSError) as e:
                print(f"[CLIENT] ⚠️  Conexión perdida con el servidor: {type(e).__name__}")
                self.running = False
                break
            except Exception as e:
                print(f"[CLIENT] ❌ Error recibiendo actualizaciones: {e}")
                self.running = False
                break
        
        print("[CLIENT] 🔌 Desconectado del servidor")
    
    def send_update(self, player_data):
        """Enviar actualización del jugador local"""
        if not self.running:
            return
        try:
            self._send_data(self.socket, {"type": "update", "player_data": player_data})
        except (ConnectionResetError, BrokenPipeError, OSError) as e:
            print(f"[CLIENT] ⚠️  Error de conexión al enviar actualización: {type(e).__name__}")
            self.running = False
        except Exception as e:
            print(f"[CLIENT] ❌ Error enviando actualización: {e}")
    
    def send_shoot(self, bullet_data):
        """Enviar evento de disparo"""
        if not self.running:
            return
        try:
            self._send_data(self.socket, {"type": "shoot", "bullet": bullet_data})
        except (ConnectionResetError, BrokenPipeError, OSError) as e:
            print(f"[CLIENT] ⚠️  Error de conexión al enviar disparo: {type(e).__name__}")
            self.running = False
        except Exception as e:
            print(f"[CLIENT] ❌ Error enviando disparo: {e}")
    
    def send_ready(self):
        """Enviar señal de listo"""
        try:
            self._send_data(self.socket, {"type": "ready"})
            print("[CLIENT] Marcado como listo")
        except Exception as e:
            print(f"[ERROR] Error enviando ready: {e}")
    
    def _send_data(self, sock, data):
        """Enviar datos serializados por socket"""
        try:
            if sock.fileno() == -1:
                return False
            serialized = pickle.dumps(data)
            size = len(serialized)
            sock.sendall(size.to_bytes(4, byteorder='big'))
            sock.sendall(serialized)
            return True
        except:
            return False
    
    def _receive_data(self, sock):
        """Recibir datos serializados del socket"""
        try:
            size_data = sock.recv(4)
            if not size_data or len(size_data) < 4:
                return None
            size = int.from_bytes(size_data, byteorder='big')
            
            if size > 10 * 1024 * 1024:
                return None
            
            data = b""
            while len(data) < size:
                chunk = sock.recv(min(size - len(data), 4096))
                if not chunk:
                    return None
                data += chunk
            
            return pickle.loads(data)
        except:
            return None
    
    def get_game_state(self):
        """Obtener el estado actual del juego"""
        with self.lock:
            return self.game_state
    
    def set_receive_callback(self, callback):
        """Establecer callback para cuando se reciben actualizaciones"""
        self.receive_callback = callback
    
    def shutdown(self):
        """Cerrar conexión"""
        self.running = False
        if self.socket:
            try:
                self.socket.close()
            except:
                pass
        print("[CLIENT] Conexión cerrada")
