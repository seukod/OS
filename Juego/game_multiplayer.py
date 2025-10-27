import pygame
import sys
import random
from enum import Enum
from network import NetworkManager, NetworkRole

# Inicializar Pygame
pygame.init()

# Constantes del juego
SCREEN_WIDTH = 800
SCREEN_HEIGHT = 600
FPS = 60
TILE_SIZE = 32

# Colores mejorados por jugador
PLAYER_COLORS = [
    (34, 139, 34),    # Verde bosque - Jugador 1 (Host)
    (30, 144, 255),   # Azul dodger - Jugador 2
    (255, 140, 0),    # Naranja oscuro - Jugador 3
    (218, 112, 214),  # Orquídea - Jugador 4
]

BLACK = (20, 20, 30)
WHITE = (240, 240, 245)
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
        return {
            "x": self.x,
            "y": self.y,
            "direction": self.direction.value,
            "lives": self.lives,
            "is_alive": self.is_alive,
            "player_id": self.player_id
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
            "active": self.active
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

class Base:
    def __init__(self, x, y):
        self.x = x
        self.y = y
        self.width = TILE_SIZE * 2
        self.height = TILE_SIZE * 2
        self.destroyed = False
    
    def get_rect(self):
        return pygame.Rect(self.x, self.y, self.width, self.height)
    
    def draw(self, screen):
        if not self.destroyed:
            pygame.draw.rect(screen, STEEL_BLUE, self.get_rect())
            pygame.draw.rect(screen, CYAN, self.get_rect(), 4)
            center_x = self.x + self.width // 2
            center_y = self.y + self.height // 2
            pygame.draw.circle(screen, CYAN, (center_x, center_y), 8)
            pygame.draw.circle(screen, STEEL_BLUE, (center_x, center_y), 5)

class MultiplayerGame:
    """Juego Battle City Multijugador"""
    def __init__(self, network_manager):
        self.screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
        pygame.display.set_caption("Battle City - Multiplayer P2P")
        self.clock = pygame.time.Clock()
        self.running = True
        
        self.network = network_manager
        self.player_id = network_manager.player_id
        
        # Inicializar jugadores
        self.tanks = {}
        spawn_positions = [(50, 50), (750, 50), (50, 550), (750, 550)]
        self.tanks[self.player_id] = Tank(
            spawn_positions[self.player_id][0],
            spawn_positions[self.player_id][1],
            PLAYER_COLORS[self.player_id],
            self.player_id
        )
        
        self.bullets = []
        self.walls = self.create_walls()
        self.base = Base(SCREEN_WIDTH//2 - TILE_SIZE, SCREEN_HEIGHT - TILE_SIZE*2 - 10)
        
        self.keys_pressed = set()
        self.ready = False
        self.game_started = False
        
        # Callback para actualizaciones de red
        self.network.set_receive_callback(self.on_network_update)
    
    def create_walls(self):
        walls = []
        for x in range(150, 350, TILE_SIZE):
            walls.append(Wall(x, 200))
        for x in range(450, 650, TILE_SIZE):
            walls.append(Wall(x, 300))
        for y in range(100, 200, TILE_SIZE):
            walls.append(Wall(100, y))
        for y in range(350, 450, TILE_SIZE):
            walls.append(Wall(700, y))
        for x in range(SCREEN_WIDTH//2 - TILE_SIZE*2, SCREEN_WIDTH//2 + TILE_SIZE*2, TILE_SIZE):
            walls.append(Wall(x, SCREEN_HEIGHT - TILE_SIZE*3))
        return walls
    
    def on_network_update(self, game_state):
        """Callback cuando se reciben actualizaciones de red"""
        # Actualizar tanques de otros jugadores
        for pid, player_data in game_state.players.items():
            if pid != self.player_id:
                if pid not in self.tanks:
                    self.tanks[pid] = Tank.from_dict(player_data, PLAYER_COLORS[pid])
                else:
                    tank = self.tanks[pid]
                    tank.x = player_data["x"]
                    tank.y = player_data["y"]
                    tank.direction = Direction(player_data["direction"])
                    tank.lives = player_data["lives"]
                    tank.is_alive = player_data["is_alive"]
        
        # Actualizar balas
        self.bullets = [Bullet.from_dict(b) for b in game_state.bullets if b["active"]]
        
        # Actualizar estado del juego
        self.game_started = game_state.game_started
        self.base.destroyed = game_state.base_destroyed
    
    def handle_events(self):
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                self.running = False
            elif event.type == pygame.KEYDOWN:
                self.keys_pressed.add(event.key)
                if event.key == pygame.K_SPACE:
                    bullet = self.tanks[self.player_id].shoot()
                    if bullet:
                        self.bullets.append(bullet)
                        self.network.send_shoot(bullet.to_dict())
                elif event.key == pygame.K_r and not self.ready:
                    self.ready = True
                    self.network.send_ready()
            elif event.type == pygame.KEYUP:
                self.keys_pressed.discard(event.key)
    
    def update(self):
        if not self.game_started:
            return
        
        my_tank = self.tanks[self.player_id]
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
        
        # Actualizar balas (solo el host procesa colisiones)
        if self.network.role == NetworkRole.HOST:
            for bullet in self.bullets[:]:
                bullet.update()
                if not bullet.active:
                    self.bullets.remove(bullet)
                    continue
                
                # Colisiones con muros
                for wall in self.walls:
                    if not wall.destroyed and bullet.get_rect().colliderect(wall.get_rect()):
                        wall.destroyed = True
                        bullet.active = False
                        break
                
                # Colisiones con tanques
                for tank in self.tanks.values():
                    if tank.is_alive and tank.player_id != bullet.owner_id:
                        if bullet.get_rect().colliderect(tank.get_rect()):
                            tank.lives -= 1
                            if tank.lives <= 0:
                                tank.is_alive = False
                            bullet.active = False
                            break
                
                # Colisión con base
                if bullet.get_rect().colliderect(self.base.get_rect()):
                    self.base.destroyed = True
                    bullet.active = False
    
    def draw(self):
        self.screen.fill(BACKGROUND)
        
        # Cuadrícula
        grid_color = (25, 35, 45)
        for x in range(0, SCREEN_WIDTH, TILE_SIZE):
            pygame.draw.line(self.screen, grid_color, (x, 0), (x, SCREEN_HEIGHT), 1)
        for y in range(0, SCREEN_HEIGHT, TILE_SIZE):
            pygame.draw.line(self.screen, grid_color, (0, y), (SCREEN_WIDTH, y), 1)
        
        # Muros y base
        for wall in self.walls:
            wall.draw(self.screen)
        self.base.draw(self.screen)
        
        # Tanques
        for tank in self.tanks.values():
            tank.draw(self.screen)
        
        # Balas
        for bullet in self.bullets:
            bullet.draw(self.screen)
        
        # UI
        font = pygame.font.Font(None, 28)
        
        if not self.game_started:
            # Pantalla de espera
            title = font.render("Esperando jugadores...", True, WHITE)
            self.screen.blit(title, (SCREEN_WIDTH//2 - 150, 100))
            
            ready_text = "Presiona R cuando estés listo" if not self.ready else "LISTO!"
            ready_surf = font.render(ready_text, True, BRIGHT_YELLOW if not self.ready else PLAYER_COLORS[0])
            self.screen.blit(ready_surf, (SCREEN_WIDTH//2 - 150, 150))
        else:
            # Info de jugadores
            y_offset = 10
            for pid, tank in self.tanks.items():
                color = PLAYER_COLORS[pid]
                status = "VIVO" if tank.is_alive else "MUERTO"
                text = font.render(f"P{pid+1}: {tank.lives} vidas - {status}", True, color)
                self.screen.blit(text, (10, y_offset))
                y_offset += 30
        
        # Instrucciones
        font_small = pygame.font.Font(None, 20)
        instructions = ["Flechas: Mover", "Espacio: Disparar", f"Tu ID: Jugador {self.player_id+1}"]
        for i, inst in enumerate(instructions):
            text = font_small.render(inst, True, WHITE)
            self.screen.blit(text, (SCREEN_WIDTH - 150, 10 + i * 20))
        
        pygame.display.flip()
    
    def run(self):
        while self.running:
            self.handle_events()
            self.update()
            self.draw()
            self.clock.tick(FPS)
        
        self.network.shutdown()
        pygame.quit()
        sys.exit()

def main_menu():
    """Menú principal para elegir rol"""
    screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
    pygame.display.set_caption("Battle City - Menú Principal")
    clock = pygame.time.Clock()
    
    font_title = pygame.font.Font(None, 64)
    font_option = pygame.font.Font(None, 36)
    
    options = [
        {"text": "1. HOSTEAR PARTIDA", "role": NetworkRole.HOST},
        {"text": "2. UNIRSE A PARTIDA", "role": NetworkRole.CLIENT},
        {"text": "3. SALIR", "role": None}
    ]
    
    selected = 0
    host_input = ""
    entering_ip = False
    
    while True:
        screen.fill(BACKGROUND)
        
        # Título
        title = font_title.render("BATTLE CITY", True, BRIGHT_YELLOW)
        screen.blit(title, (SCREEN_WIDTH//2 - 180, 100))
        
        subtitle = font_option.render("Multiplayer P2P", True, WHITE)
        screen.blit(subtitle, (SCREEN_WIDTH//2 - 100, 170))
        
        # Opciones
        for i, option in enumerate(options):
            color = PLAYER_COLORS[0] if i == selected else WHITE
            text = font_option.render(option["text"], True, color)
            screen.blit(text, (SCREEN_WIDTH//2 - 150, 250 + i * 50))
        
        # Si está ingresando IP
        if entering_ip:
            prompt = font_option.render("IP del host:", True, WHITE)
            screen.blit(prompt, (SCREEN_WIDTH//2 - 100, 400))
            ip_text = font_option.render(host_input + "_", True, BRIGHT_YELLOW)
            screen.blit(ip_text, (SCREEN_WIDTH//2 - 100, 440))
            hint = font_option.render("(Enter para conectar, Esc para cancelar)", True, WHITE)
            font_small = pygame.font.Font(None, 20)
            hint_small = font_small.render("Deja vacío para localhost", True, WHITE)
            screen.blit(hint_small, (SCREEN_WIDTH//2 - 120, 480))
        
        pygame.display.flip()
        
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()
            
            if entering_ip:
                if event.type == pygame.KEYDOWN:
                    if event.key == pygame.K_RETURN:
                        host = host_input if host_input else "localhost"
                        network = NetworkManager(NetworkRole.CLIENT, host=host)
                        if network.start_client():
                            return network
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
                        role = options[selected]["role"]
                        if role == NetworkRole.HOST:
                            network = NetworkManager(role)
                            if network.start_host():
                                return network
                        elif role == NetworkRole.CLIENT:
                            entering_ip = True
                        else:
                            pygame.quit()
                            sys.exit()
        
        clock.tick(30)

if __name__ == "__main__":
    network = main_menu()
    game = MultiplayerGame(network)
    game.run()
