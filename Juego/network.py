"""
"""
"""
Sistema de networking Cliente-Servidor para Battle City Multiplayer
Servidor dedicado que maneja hasta 4 clientes jugadores
"""
import socket
import threading
import pickle
import time
import logging
import os
from datetime import datetime
from enum import Enum

# Importar configuración de mapas
try:
    from maps import CURRENT_MAP
except ImportError:
    # Fallback si no encuentra el archivo
    CURRENT_MAP = [[0] * 25 for _ in range(18)]

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
        self.victory_logged = False  # Para evitar log duplicado
    
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
        self._setup_logging()  # Configurar sistema de logs primero
        self._initialize_walls()  # Inicializar muros después
        
    def _setup_logging(self):
        """Configurar sistema de logging"""
        # Obtener el directorio del script actual (Juego/)
        script_dir = os.path.dirname(os.path.abspath(__file__))
        logs_dir = os.path.join(script_dir, "logs")
        
        # Crear carpeta de logs si no existe
        if not os.path.exists(logs_dir):
            os.makedirs(logs_dir)
        
        # Crear logger
        self.logger = logging.getLogger("GameServer")
        self.logger.setLevel(logging.DEBUG)
        
        # Archivo de log con timestamp
        timestamp = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
        log_filename = os.path.join(logs_dir, f"game_{timestamp}.log")
        
        # Handler para archivo
        file_handler = logging.FileHandler(log_filename)
        file_handler.setLevel(logging.DEBUG)
        
        # Handler para consola
        console_handler = logging.StreamHandler()
        console_handler.setLevel(logging.INFO)
        
        # Formato del log
        formatter = logging.Formatter(
            '[%(asctime)s] %(levelname)s - %(message)s',
            datefmt='%H:%M:%S'
        )
        file_handler.setFormatter(formatter)
        console_handler.setFormatter(formatter)
        
        # Agregar handlers
        self.logger.addHandler(file_handler)
        self.logger.addHandler(console_handler)
        
        self.logger.info("="*60)
        self.logger.info("SERVIDOR DE JUEGO INICIADO")
        self.logger.info(f"Archivo de log: {log_filename}")
        self.logger.info("="*60)
        
    def _initialize_walls(self):
        """Inicializar los muros del servidor desde la matriz (sincronizado con cliente)"""
        TILE_SIZE = 32
        walls = []
        
        # Generar muros desde la matriz importada
        for row_idx, row in enumerate(CURRENT_MAP):
            for col_idx, cell in enumerate(row):
                if cell == 1:  # 1 = Muro
                    x = col_idx * TILE_SIZE
                    y = row_idx * TILE_SIZE
                    walls.append({
                        "x": x,
                        "y": y,
                        "width": TILE_SIZE,
                        "height": TILE_SIZE
                    })
        
        self.game_state.walls = walls
        self.logger.info(f"Mapa inicializado: {len(walls)} muros generados desde matriz")
        
    def start(self):
        """Iniciar el servidor dedicado"""
        try:
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            self.socket.bind(("0.0.0.0", self.port))
            self.socket.listen(self.max_players)
            self.running = True
            
            local_ip = get_local_ip()
            
            self.logger.info("="*60)
            self.logger.info("SERVIDOR DEDICADO INICIADO")
            self.logger.info("="*60)
            self.logger.info(f"Puerto: {self.port}")
            self.logger.info(f"Máximo jugadores: {self.max_players}")
            self.logger.info(f"IP local: {local_ip}")
            self.logger.info(f"Los jugadores deben conectarse a: {local_ip}")
            self.logger.info("="*60)
            self.logger.info("Esperando jugadores...")
            
            accept_thread = threading.Thread(target=self._accept_connections, daemon=True)
            accept_thread.start()
            
            # Iniciar el game loop del servidor
            game_loop_thread = threading.Thread(target=self._game_loop, daemon=True)
            game_loop_thread.start()
            
            return True
        except Exception as e:
            self.logger.error(f"No se pudo iniciar el servidor: {e}")
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
                
                self.logger.info(f"✓ Jugador {player_id + 1} conectado desde {address[0]}:{address[1]}")
                self.logger.info(f"Jugadores conectados: {len(self.clients)}/{self.max_players}")
                
                client_thread = threading.Thread(
                    target=self._handle_client,
                    args=(client_socket, player_id),
                    daemon=True
                )
                client_thread.start()
                
            except Exception as e:
                if self.running:
                    self.logger.error(f"Error aceptando conexión: {e}")
    
    def _handle_client(self, client_socket, player_id):
        """Manejar mensajes de un cliente"""
        while self.running:
            try:
                data = self._receive_data(client_socket)
                if not data:
                    self.logger.warning(f"Jugador {player_id + 1} perdió la conexión (sin datos)")
                    break
                
                if data["type"] == "update":
                    with self.lock:
                        self.game_state.players[player_id] = data["player_data"]
                        self._broadcast_state()
                
                elif data["type"] == "shoot":
                    with self.lock:
                        bullet_data = data["bullet"]
                        player_team = self.game_state.players[player_id].get('team', 0)
                        self.logger.info(f"🔫 Jugador {player_id + 1} disparó en ({bullet_data['x']}, {bullet_data['y']}) dirección {bullet_data['direction']}")
                        # Log estructurado para estadísticas
                        self.logger.info(f"SHOT_FIRED|player_id={player_id}|team={player_team}|x={bullet_data['x']}|y={bullet_data['y']}")
                        self.game_state.bullets.append(bullet_data)
                        self._broadcast_state()
                
                elif data["type"] == "ready":
                    with self.lock:
                        self.game_state.players_ready.add(player_id)
                        self.logger.info(f"✓ Jugador {player_id + 1} está listo ({len(self.game_state.players_ready)}/{len(self.clients)})")
                        if len(self.game_state.players_ready) == len(self.clients):
                            self.game_state.game_started = True
                            self.logger.info("🎮 JUEGO INICIADO - Todos los jugadores están listos")
                            # Log estructurado para estadísticas
                            team_blue = [pid for pid, p in self.game_state.players.items() if p.get('team') == 0]
                            team_red = [pid for pid, p in self.game_state.players.items() if p.get('team') == 1]
                            self.logger.info(f"GAME_START|team_blue={len(team_blue)}|team_red={len(team_red)}|total_players={len(self.game_state.players)}")
                        self._broadcast_state()
                        
            except (ConnectionResetError, BrokenPipeError, OSError) as e:
                self.logger.warning(f"Jugador {player_id + 1} desconectado inesperadamente: {type(e).__name__}")
                break
            except Exception as e:
                self.logger.error(f"Error manejando cliente {player_id + 1}: {e}")
                break
        
        # Limpieza al desconectar
        self.logger.info(f"🔌 Jugador {player_id + 1} desconectado - Limpiando...")
        with self.lock:
            # Remover de lista de clientes
            self.clients = [c for c in self.clients if c["id"] != player_id]
            
            # Si el jugador estaba en el juego, marcarlo como muerto
            if player_id in self.game_state.players:
                if self.game_state.game_started:
                    # Durante el juego, marcar como muerto pero mantener en el estado
                    self.game_state.players[player_id]["is_alive"] = False
                    self.game_state.players[player_id]["lives"] = 0
                    self.logger.warning(f"💀 Jugador {player_id + 1} marcado como eliminado (desconexión)")
                    
                    # Verificar si esto causa victoria
                    if self.game_state.check_victory() and not self.game_state.victory_logged:
                        self.game_state.victory_logged = True
                        self.logger.critical(f"🏆 ¡EQUIPO {self.game_state.winning_team + 1} GANA por desconexión del enemigo!")
                        self.logger.critical(f"GAME_END|winner_team={self.game_state.winning_team}|team_name={'AZUL' if self.game_state.winning_team == 0 else 'ROJO'}")
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
        
        self.logger.debug(f"✓ Limpieza completada para Jugador {player_id + 1}")
    
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
                self.logger.warning(f"Error enviando a Jugador {client['id'] + 1}: {type(e).__name__}")
                clients_to_remove.append(client["id"])
            except Exception as e:
                self.logger.error(f"Error inesperado con Jugador {client['id'] + 1}: {e}")
                clients_to_remove.append(client["id"])
        
        if clients_to_remove:
            self.logger.debug(f"🧹 Limpiando {len(clients_to_remove)} cliente(s) desconectado(s)")
            with self.lock:
                self.clients = [c for c in self.clients if c["id"] not in clients_to_remove]
                for client_id in clients_to_remove:
                    if client_id in self.game_state.players:
                        if self.game_state.game_started:
                            # Durante el juego, marcar como muerto
                            self.game_state.players[client_id]["is_alive"] = False
                            self.game_state.players[client_id]["lives"] = 0
                            self.logger.warning(f"💀 Jugador {client_id + 1} marcado como eliminado (broadcast)")
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
                                self.logger.debug(f"💥 Bala impactó un muro en ({wall['x']}, {wall['y']})")
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
                                owner_name = f"Jugador {bullet['owner_id'] + 1}"
                                target_name = f"Jugador {player_id + 1}"
                                player_data["lives"] = player_data.get("lives", 3) - 1
                                self.logger.info(f"🎯 {owner_name} impactó a {target_name}! Vidas restantes: {player_data['lives']}")
                                if player_data["lives"] <= 0:
                                    player_data["is_alive"] = False
                                    owner_team = self.game_state.players[bullet['owner_id']].get('team', 0)
                                    target_team = player_data.get('team', 0)
                                    self.logger.warning(f"💀 {target_name} ELIMINADO por {owner_name}")
                                    # Log estructurado para estadísticas
                                    self.logger.warning(f"PLAYER_KILLED|killer_id={bullet['owner_id']}|killer_team={owner_team}|victim_id={player_id}|victim_team={target_team}")
                                
                                # Desactivar bala
                                bullet["active"] = False
                                break
                    
                    # Limpiar balas inactivas
                    active_before = len(self.game_state.bullets)
                    self.game_state.bullets = [b for b in self.game_state.bullets if b.get("active", True)]
                    active_after = len(self.game_state.bullets)
                    
                    # Verificar victoria (registrar solo una vez)
                    if self.game_state.check_victory() and not self.game_state.victory_logged:
                        self.game_state.victory_logged = True
                        self.logger.critical(f"🏆 ¡EQUIPO {self.game_state.winning_team + 1} GANA LA PARTIDA!")
                        # Log estructurado para estadísticas
                        self.logger.critical(f"GAME_END|winner_team={self.game_state.winning_team}|team_name={'AZUL' if self.game_state.winning_team == 0 else 'ROJO'}")
                    
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
        self.logger.info("Servidor ejecutándose. Presiona Ctrl+C para detener.")
        try:
            while self.running:
                time.sleep(1)
        except KeyboardInterrupt:
            self.logger.info("Deteniendo servidor...")
            self.shutdown()
    
    def shutdown(self):
        """Cerrar servidor"""
        self.logger.info("Cerrando servidor...")
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
        self.logger.info("="*60)
        self.logger.info("SERVIDOR DETENIDO")
        self.logger.info("="*60)
        # Cerrar handlers de logging
        for handler in self.logger.handlers[:]:
            handler.close()
            self.logger.removeHandler(handler)

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
