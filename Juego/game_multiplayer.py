import pygame
import sys
import random
import time
from enum import Enum
from network import NetworkManager, DedicatedServer
from maps import CURRENT_MAP, SPAWN_POSITIONS
import threading

# Inicializar Pygame
pygame.init()

# Constantes del juego
SCREEN_WIDTH = 800
SCREEN_HEIGHT = 600
FPS = 60
TILE_SIZE = 32

# Colores por equipo
# EQUIPO 0 (Azul): Jugadores 0 y 1
# EQUIPO 1 (Rojo): Jugadores 2 y 3
PLAYER_COLORS = [
    (30, 144, 255),   # Azul claro - Jugador 1 (Equipo Azul)
    (70, 130, 180),   # Azul acero - Jugador 2 (Equipo Azul)
    (220, 20, 60),    # Rojo carmesí - Jugador 3 (Equipo Rojo)
    (255, 69, 0),     # Rojo naranja - Jugador 4 (Equipo Rojo)
]

TEAM_COLORS = [
    (30, 144, 255),   # Equipo 0 - Azul
    (220, 20, 60),    # Equipo 1 - Rojo
]

BLACK = (20, 20, 30)
WHITE = (240, 240, 245)
RED = (178, 34, 34)
DARK_RED = (178, 34, 34)
STEEL_BLUE = (70, 130, 180)
CYAN = (100, 200, 255)
BRICK_ORANGE = (205, 92, 92)
DARK_BROWN = (101, 67, 33)
BRIGHT_YELLOW = (255, 215, 0)
BACKGROUND = (15, 25, 35)

# Direcciones
class Direction(Enum):
    UP = 0
    DOWN = 1
    LEFT = 2
    RIGHT = 3

class Tank:
    """Clase base para todos los tanques"""
    def __init__(self, x, y, color, player_id=0, direction=Direction.UP):
        self.x = x
        self.y = y
        self.width = TILE_SIZE
        self.height = TILE_SIZE
        self.color = color
        self.direction = direction
        self.speed = 2
        self.last_shot = 0
        self.shot_delay = 500
        self.player_id = player_id
        self.lives = 3
        self.is_alive = True
        
    def get_rect(self):
        return pygame.Rect(self.x, self.y, self.width, self.height)
    
    def draw(self, screen):
        if not self.is_alive:
            return
            
        pygame.draw.rect(screen, self.color, self.get_rect())
        light_color = tuple(min(255, c + 40) for c in self.color)
        pygame.draw.rect(screen, light_color, self.get_rect(), 2)
        
        center_x = self.x + self.width // 2
        center_y = self.y + self.height // 2
        
        if self.direction == Direction.UP:
            end_x, end_y = center_x, self.y
        elif self.direction == Direction.DOWN:
            end_x, end_y = center_x, self.y + self.height
        elif self.direction == Direction.LEFT:
            end_x, end_y = self.x, center_y
        elif self.direction == Direction.RIGHT:
            end_x, end_y = self.x + self.width, center_y
        
        pygame.draw.line(screen, BLACK, (center_x, center_y), (end_x, end_y), 5)
        pygame.draw.line(screen, light_color, (center_x, center_y), (end_x, end_y), 3)
        
        # Dibujar número de jugador
        font = pygame.font.Font(None, 20)
        text = font.render(str(self.player_id + 1), True, WHITE)
        text_rect = text.get_rect(center=(center_x, center_y))
        screen.blit(text, text_rect)
    
    def can_shoot(self):
        current_time = pygame.time.get_ticks()
        return current_time - self.last_shot >= self.shot_delay
    
    def shoot(self):
        if self.can_shoot() and self.is_alive:
            self.last_shot = pygame.time.get_ticks()
            bullet_x = self.x + self.width // 2 - 2
            bullet_y = self.y + self.height // 2 - 2
            return Bullet(bullet_x, bullet_y, self.direction, self.player_id)
        return None
    
    def move(self, direction, walls, other_tanks):
        if not self.is_alive:
            return
            
        old_x, old_y = self.x, self.y
        self.direction = direction
        
        if direction == Direction.UP:
            self.y -= self.speed
        elif direction == Direction.DOWN:
            self.y += self.speed
        elif direction == Direction.LEFT:
            self.x -= self.speed
        elif direction == Direction.RIGHT:
            self.x += self.speed
        
        if self.x < 0 or self.x + self.width > SCREEN_WIDTH:
            self.x = old_x
        if self.y < 0 or self.y + self.height > SCREEN_HEIGHT:
            self.y = old_y
        
        if self.check_collision_with_walls(walls):
            self.x, self.y = old_x, old_y
        
        if self.check_collision_with_tanks(other_tanks):
            self.x, self.y = old_x, old_y
    
    def check_collision_with_walls(self, walls):
        tank_rect = self.get_rect()
        for wall in walls:
            if not wall.destroyed and tank_rect.colliderect(wall.get_rect()):
                return True
        return False
    
    def check_collision_with_tanks(self, tanks):
        tank_rect = self.get_rect()
        for tank in tanks:
            if tank.player_id != self.player_id and tank.is_alive and tank_rect.colliderect(tank.get_rect()):
                return True
        return False
    
    def to_dict(self):
        # Determinar equipo según player_id: 0,1 = Equipo 0 (AZUL), 2,3 = Equipo 1 (ROJO)
        team = 0 if self.player_id in [0, 1] else 1
        return {
            "x": self.x,
            "y": self.y,
            "direction": self.direction.value,
            "lives": self.lives,
            "is_alive": self.is_alive,
            "player_id": self.player_id,
            "team": team
        }
    
    @staticmethod
    def from_dict(data, color):
        tank = Tank(data["x"], data["y"], color, data["player_id"])
        tank.direction = Direction(data["direction"])
        tank.lives = data["lives"]
        tank.is_alive = data["is_alive"]
        return tank

class Bullet:
    def __init__(self, x, y, direction, owner_id):
        self.x = x
        self.y = y
        self.width = 4
        self.height = 4
        self.direction = direction
        self.speed = 5
        self.active = True
        self.owner_id = owner_id
    
    def update(self):
        if not self.active:
            return
        
        if self.direction == Direction.UP:
            self.y -= self.speed
        elif self.direction == Direction.DOWN:
            self.y += self.speed
        elif self.direction == Direction.LEFT:
            self.x -= self.speed
        elif self.direction == Direction.RIGHT:
            self.x += self.speed
        
        if (self.x < 0 or self.x > SCREEN_WIDTH or 
            self.y < 0 or self.y > SCREEN_HEIGHT):
            self.active = False
    
    def get_rect(self):
        return pygame.Rect(self.x, self.y, self.width, self.height)
    
    def draw(self, screen):
        if self.active:
            pygame.draw.rect(screen, BRIGHT_YELLOW, self.get_rect())
            pygame.draw.rect(screen, WHITE, (self.x + 1, self.y + 1, 2, 2))
    
    def to_dict(self):
        return {
            "x": self.x,
            "y": self.y,
            "direction": self.direction.value,
            "owner_id": self.owner_id,
            "active": self.active,
            "width": self.width,
            "height": self.height,
            "speed": self.speed
        }
    
    @staticmethod
    def from_dict(data):
        bullet = Bullet(data["x"], data["y"], Direction(data["direction"]), data["owner_id"])
        bullet.active = data["active"]
        return bullet

class Wall:
    def __init__(self, x, y):
        self.x = x
        self.y = y
        self.width = TILE_SIZE
        self.height = TILE_SIZE
        self.destroyed = False
    
    def get_rect(self):
        return pygame.Rect(self.x, self.y, self.width, self.height)
    
    def draw(self, screen):
        if not self.destroyed:
            pygame.draw.rect(screen, BRICK_ORANGE, self.get_rect())
            pygame.draw.rect(screen, DARK_BROWN, self.get_rect(), 2)
            pygame.draw.line(screen, DARK_BROWN, (self.x, self.y + self.height//2), 
                           (self.x + self.width, self.y + self.height//2), 1)
            pygame.draw.line(screen, DARK_BROWN, (self.x + self.width//2, self.y), 
                           (self.x + self.width//2, self.y + self.height), 1)

class MultiplayerGame:
    """Juego Battle City Multijugador"""
    def __init__(self, network_manager):
        self.screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
        pygame.display.set_caption("Battle City - Multiplayer Cliente-Servidor")
        self.clock = pygame.time.Clock()
        self.running = True
        self.lock = threading.Lock()  # Lock para operaciones thread-safe
        
        self.network = network_manager
        self.player_id = network_manager.player_id
        
        # Inicializar jugadores desde la matriz de spawn
        self.tanks = {}
        spawn_row, spawn_col = SPAWN_POSITIONS[self.player_id]
        spawn_x = spawn_col * TILE_SIZE
        spawn_y = spawn_row * TILE_SIZE
        
        self.tanks[self.player_id] = Tank(
            spawn_x,
            spawn_y,
            PLAYER_COLORS[self.player_id],
            self.player_id
        )
        
        self.bullets = []
        self.walls = self.create_walls_from_matrix()
        
        self.keys_pressed = set()
        self.ready = False
        self.game_started = False
        
        # Callback para actualizaciones de red
        self.network.set_receive_callback(self.on_network_update)
    
    def create_walls_from_matrix(self):
        """Crea muros a partir de la matriz importada desde maps.py"""
        walls = []
        for row_idx, row in enumerate(CURRENT_MAP):
            for col_idx, cell in enumerate(row):
                if cell == 1:  # 1 = Muro
                    x = col_idx * TILE_SIZE
                    y = row_idx * TILE_SIZE
                    walls.append(Wall(x, y))
        return walls
    
    def on_network_update(self, game_state):
        """Callback cuando se reciben actualizaciones de red"""
        with self.lock:
            # Actualizar TODOS los tanques (incluyendo el local)
            for pid, player_data in game_state.players.items():
                if pid not in self.tanks:
                    self.tanks[pid] = Tank.from_dict(player_data, PLAYER_COLORS[pid])
                else:
                    tank = self.tanks[pid]
                    # Actualizar posición solo para otros jugadores
                    # El jugador local maneja su propia posición
                    if pid != self.player_id:
                        tank.x = player_data["x"]
                        tank.y = player_data["y"]
                        tank.direction = Direction(player_data["direction"])
                    # Actualizar vidas y estado para TODOS (incluyendo el local)
                    tank.lives = player_data["lives"]
                    tank.is_alive = player_data["is_alive"]
            
            # Actualizar balas
            self.bullets = [Bullet.from_dict(b) for b in game_state.bullets if b["active"]]
            
            # Actualizar estado del juego
            self.game_started = game_state.game_started
    
    def handle_events(self):
        # Obtener game_state para verificar si el juego terminó
        game_state = None
        if hasattr(self.network, 'last_game_state'):
            game_state = self.network.last_game_state
        
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                self.running = False
            elif event.type == pygame.KEYDOWN:
                # Si el juego terminó, solo permitir ESC para salir
                if game_state and game_state.game_over:
                    if event.key == pygame.K_ESCAPE:
                        self.running = False
                    continue  # No procesar otras teclas durante game over
                
                self.keys_pressed.add(event.key)
                if event.key == pygame.K_SPACE:
                    bullet = self.tanks[self.player_id].shoot()
                    if bullet:
                        self.bullets.append(bullet)
                        self.network.send_shoot(bullet.to_dict())
                elif event.key == pygame.K_r and not self.ready:
                    self.ready = True
                    self.network.send_ready()
                elif event.key == pygame.K_ESCAPE:
                    # ESC durante el juego también sale
                    self.running = False
            elif event.type == pygame.KEYUP:
                self.keys_pressed.discard(event.key)
    
    def update(self):
        # Mantener la conexión activa incluso en el lobby
        if self.player_id not in self.tanks:
            return  # Esperar a que el tanque esté inicializado
            
        my_tank = self.tanks[self.player_id]
        
        if not self.game_started:
            # En el lobby, solo enviar estado básico para mantener conexión
            if hasattr(self, '_last_lobby_update'):
                if pygame.time.get_ticks() - self._last_lobby_update > 1000:  # Cada segundo
                    self.network.send_update(my_tank.to_dict())
                    self._last_lobby_update = pygame.time.get_ticks()
            else:
                self._last_lobby_update = pygame.time.get_ticks()
            return
        
        # Obtener otros tanques de forma thread-safe
        with self.lock:
            other_tanks = [t for pid, t in self.tanks.items() if pid != self.player_id]
        
        # Mover tanque local
        if pygame.K_UP in self.keys_pressed:
            my_tank.move(Direction.UP, self.walls, other_tanks)
        elif pygame.K_DOWN in self.keys_pressed:
            my_tank.move(Direction.DOWN, self.walls, other_tanks)
        elif pygame.K_LEFT in self.keys_pressed:
            my_tank.move(Direction.LEFT, self.walls, other_tanks)
        elif pygame.K_RIGHT in self.keys_pressed:
            my_tank.move(Direction.RIGHT, self.walls, other_tanks)
        
        # Enviar actualización de posición
        self.network.send_update(my_tank.to_dict())
        
        # Las balas y colisiones las maneja el servidor
        # El cliente solo actualiza visualmente basado en el estado recibido
    
    def draw(self):
        self.screen.fill(BACKGROUND)
        
        # Cuadrícula
        grid_color = (25, 35, 45)
        for x in range(0, SCREEN_WIDTH, TILE_SIZE):
            pygame.draw.line(self.screen, grid_color, (x, 0), (x, SCREEN_HEIGHT), 1)
        for y in range(0, SCREEN_HEIGHT, TILE_SIZE):
            pygame.draw.line(self.screen, grid_color, (0, y), (SCREEN_WIDTH, y), 1)
        
        # Muros
        for wall in self.walls:
            wall.draw(self.screen)
        
        # Tanques (thread-safe)
        with self.lock:
            tanks_snapshot = list(self.tanks.values())
        for tank in tanks_snapshot:
            tank.draw(self.screen)
        
        # Balas
        for bullet in self.bullets:
            bullet.draw(self.screen)
        
        # UI
        font = pygame.font.Font(None, 28)
        
        # Obtener game_state del servidor
        game_state = None
        if hasattr(self.network, 'last_game_state'):
            game_state = self.network.last_game_state
        
        if not self.game_started:
            # Pantalla de espera
            title = font.render("Esperando jugadores...", True, WHITE)
            self.screen.blit(title, (SCREEN_WIDTH//2 - 150, 100))
            
            ready_text = "Presiona R cuando estés listo" if not self.ready else "LISTO!"
            ready_surf = font.render(ready_text, True, BRIGHT_YELLOW if not self.ready else PLAYER_COLORS[0])
            self.screen.blit(ready_surf, (SCREEN_WIDTH//2 - 150, 150))
        elif game_state and game_state.game_over:
            # Pantalla de victoria/derrota
            font_big = pygame.font.Font(None, 72)
            font_medium = pygame.font.Font(None, 36)
            team_name = "EQUIPO AZUL" if game_state.winning_team == 0 else "EQUIPO ROJO"
            team_color = TEAM_COLORS[game_state.winning_team]
            
            # Fondo semi-transparente
            overlay = pygame.Surface((SCREEN_WIDTH, SCREEN_HEIGHT))
            overlay.set_alpha(200)
            overlay.fill((0, 0, 0))
            self.screen.blit(overlay, (0, 0))
            
            # Mensaje de victoria
            victory_text = font_big.render(f"¡{team_name} GANA!", True, team_color)
            text_rect = victory_text.get_rect(center=(SCREEN_WIDTH//2, SCREEN_HEIGHT//2 - 80))
            self.screen.blit(victory_text, text_rect)
            
            # Mi resultado
            my_team = 0 if self.player_id in [0, 1] else 1
            if my_team == game_state.winning_team:
                result_text = font.render("¡VICTORIA!", True, BRIGHT_YELLOW)
            else:
                result_text = font.render("DERROTA", True, RED)
            result_rect = result_text.get_rect(center=(SCREEN_WIDTH//2, SCREEN_HEIGHT//2 - 10))
            self.screen.blit(result_text, result_rect)
            
            # Opción para salir
            options_y = SCREEN_HEIGHT//2 + 80
            option_text = font_medium.render("ESC - Salir", True, WHITE)
            option_rect = option_text.get_rect(center=(SCREEN_WIDTH//2, options_y))
            self.screen.blit(option_text, option_rect)
        else:
            # Info de equipos durante el juego
            y_offset = 10
            with self.lock:
                tanks_info = [(pid, tank.lives, tank.is_alive) for pid, tank in self.tanks.items()]
            
            # Equipo Azul (jugadores 0 y 1)
            team_blue_text = font.render("EQUIPO AZUL", True, TEAM_COLORS[0])
            self.screen.blit(team_blue_text, (10, y_offset))
            y_offset += 30
            
            for pid, lives, is_alive in tanks_info:
                if pid in [0, 1]:
                    color = PLAYER_COLORS[pid]
                    status = "VIVO" if is_alive else "MUERTO"
                    text = font.render(f"  P{pid+1}: {lives} vidas - {status}", True, color)
                    self.screen.blit(text, (10, y_offset))
                    y_offset += 25
            
            y_offset += 10
            
            # Equipo Rojo (jugadores 2 y 3)
            team_red_text = font.render("EQUIPO ROJO", True, TEAM_COLORS[1])
            self.screen.blit(team_red_text, (10, y_offset))
            y_offset += 30
            
            for pid, lives, is_alive in tanks_info:
                if pid in [2, 3]:
                    color = PLAYER_COLORS[pid]
                    status = "VIVO" if is_alive else "MUERTO"
                    text = font.render(f"  P{pid+1}: {lives} vidas - {status}", True, color)
                    self.screen.blit(text, (10, y_offset))
                    y_offset += 25
        
        # Instrucciones
        font_small = pygame.font.Font(None, 20)
        instructions = ["Flechas: Mover", "Espacio: Disparar", f"Tu ID: Jugador {self.player_id+1}"]
        for i, inst in enumerate(instructions):
            text = font_small.render(inst, True, WHITE)
            self.screen.blit(text, (SCREEN_WIDTH - 150, 10 + i * 20))
        
        # Indicador de conexión
        if not self.network.running:
            disconnect_text = font.render("⚠️  DESCONECTADO", True, RED)
            disconnect_rect = disconnect_text.get_rect(center=(SCREEN_WIDTH//2, 20))
            self.screen.blit(disconnect_text, disconnect_rect)
        
        pygame.display.flip()
    
    def run(self):
        while self.running:
            # Si se pierde la conexión, mostrar mensaje y salir
            if not self.network.running:
                print("[GAME] ⚠️  Conexión perdida. Cerrando juego...")
                self.running = False
                break
            
            self.handle_events()
            self.update()
            self.draw()
            self.clock.tick(FPS)
        
        self.network.shutdown()
        pygame.quit()
        sys.exit()

def main_menu():
    """Menú principal para elegir cómo jugar"""
    screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
    pygame.display.set_caption("Battle City - Menú Principal")
    clock = pygame.time.Clock()
    
    font_title = pygame.font.Font(None, 64)
    font_option = pygame.font.Font(None, 36)
    font_small = pygame.font.Font(None, 24)
    
    options = [
        {"text": "1. INICIAR SERVIDOR Y JUGAR", "action": "server_and_play"},
        {"text": "2. CONECTARSE A SERVIDOR", "action": "connect"},
        {"text": "3. SALIR", "action": "exit"}
    ]
    
    selected = 0
    host_input = ""
    entering_ip = False
    
    while True:
        screen.fill(BACKGROUND)
        
        # Título
        title = font_title.render("BATTLE CITY", True, BRIGHT_YELLOW)
        screen.blit(title, (SCREEN_WIDTH//2 - 180, 80))
        
        subtitle = font_option.render("Cliente-Servidor Multiplayer", True, WHITE)
        screen.blit(subtitle, (SCREEN_WIDTH//2 - 200, 150))
        
        # Opciones
        for i, option in enumerate(options):
            color = PLAYER_COLORS[0] if i == selected else WHITE
            text = font_option.render(option["text"], True, color)
            screen.blit(text, (SCREEN_WIDTH//2 - 220, 230 + i * 50))
        
        # Información
        if not entering_ip:
            info_lines = [
                "Opción 1: Inicia un servidor dedicado en segundo plano",
                "          y te conectas como jugador",
                "Opción 2: Solo te conectas a un servidor existente",
            ]
            y = 400
            for line in info_lines:
                text = font_small.render(line, True, CYAN)
                screen.blit(text, (50, y))
                y += 25
        
        # Si está ingresando IP
        if entering_ip:
            prompt = font_option.render("IP del servidor:", True, WHITE)
            screen.blit(prompt, (SCREEN_WIDTH//2 - 130, 400))
            ip_text = font_option.render(host_input + "_", True, BRIGHT_YELLOW)
            screen.blit(ip_text, (SCREEN_WIDTH//2 - 130, 440))
            hint_small = font_small.render("Presiona Enter (vacío = localhost) o Esc para cancelar", True, WHITE)
            screen.blit(hint_small, (SCREEN_WIDTH//2 - 250, 480))
        
        pygame.display.flip()
        
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()
            
            if entering_ip:
                if event.type == pygame.KEYDOWN:
                    if event.key == pygame.K_RETURN:
                        host = host_input if host_input else "localhost"
                        network = NetworkManager(host=host)
                        if network.start_client():
                            return network, None
                        else:
                            entering_ip = False
                            host_input = ""
                    elif event.key == pygame.K_ESCAPE:
                        entering_ip = False
                        host_input = ""
                    elif event.key == pygame.K_BACKSPACE:
                        host_input = host_input[:-1]
                    elif event.unicode.isprintable():
                        host_input += event.unicode
            else:
                if event.type == pygame.KEYDOWN:
                    if event.key == pygame.K_UP:
                        selected = (selected - 1) % len(options)
                    elif event.key == pygame.K_DOWN:
                        selected = (selected + 1) % len(options)
                    elif event.key == pygame.K_RETURN:
                        action = options[selected]["action"]
                        
                        if action == "server_and_play":
                            # Iniciar servidor en thread separado
                            server = DedicatedServer()
                            if server.start():
                                server_thread = threading.Thread(target=server.run_forever, daemon=True)
                                server_thread.start()
                                print("[INFO] Servidor iniciado en segundo plano")
                                time.sleep(1)  # Esperar a que el servidor esté listo
                                
                                # Conectarse como cliente
                                network = NetworkManager(host="localhost")
                                if network.start_client():
                                    return network, server
                                else:
                                    server.shutdown()
                            
                        elif action == "connect":
                            entering_ip = True
                            
                        elif action == "exit":
                            pygame.quit()
                            sys.exit()
        
        clock.tick(30)

if __name__ == "__main__":
    network, server = main_menu()
    game = MultiplayerGame(network)
    
    try:
        game.run()
    finally:
        # Cerrar servidor si existe
        if server:
            server.shutdown()
