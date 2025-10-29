import pygame
import sys
import random
from enum import Enum

# Inicializar Pygame
pygame.init()

# Constantes del juego
SCREEN_WIDTH = 800
SCREEN_HEIGHT = 600
FPS = 60
TILE_SIZE = 32

# Colores mejorados
BLACK = (20, 20, 30)  # Negro más suave
WHITE = (240, 240, 245)  # Blanco crema
DARK_GREEN = (34, 139, 34)  # Verde bosque para jugador
LIGHT_GREEN = (50, 205, 50)  # Verde claro para detalles
DARK_RED = (178, 34, 34)  # Rojo oscuro para enemigos
CRIMSON = (220, 20, 60)  # Carmesí para detalles
STEEL_BLUE = (70, 130, 180)  # Azul acero para la base
CYAN = (100, 200, 255)  # Cian para detalles de base
BRICK_ORANGE = (205, 92, 92)  # Ladrillo naranja para muros
DARK_BROWN = (101, 67, 33)  # Marrón oscuro para sombra de muros
BRIGHT_YELLOW = (255, 215, 0)  # Amarillo dorado para balas
BACKGROUND = (15, 25, 35)  # Fondo azul oscuro espacial

# Direcciones
class Direction(Enum):
    UP = 0
    DOWN = 1
    LEFT = 2
    RIGHT = 3

class Tank:
    """Clase base para todos los tanques"""
    def __init__(self, x, y, color, direction=Direction.UP):
        self.x = x
        self.y = y
        self.width = TILE_SIZE
        self.height = TILE_SIZE
        self.color = color
        self.direction = direction
        self.speed = 2
        self.last_shot = 0
        self.shot_delay = 500  # milisegundos entre disparos
        
    def get_rect(self):
        return pygame.Rect(self.x, self.y, self.width, self.height)
    
    def draw(self, screen):
        # Dibujar el tanque con efecto 3D mejorado
        # Cuerpo principal
        pygame.draw.rect(screen, self.color, self.get_rect())
        
        # Borde más claro para efecto 3D
        light_color = tuple(min(255, c + 40) for c in self.color)
        pygame.draw.rect(screen, light_color, self.get_rect(), 2)
        
        # Dibujar torreta (indicador de dirección mejorado)
        center_x = self.x + self.width // 2
        center_y = self.y + self.height // 2
        turret_length = self.width // 2 - 2
        
        if self.direction == Direction.UP:
            end_x, end_y = center_x, self.y
        elif self.direction == Direction.DOWN:
            end_x, end_y = center_x, self.y + self.height
        elif self.direction == Direction.LEFT:
            end_x, end_y = self.x, center_y
        elif self.direction == Direction.RIGHT:
            end_x, end_y = self.x + self.width, center_y
        
        # Dibujar cañón (más grueso y con sombra)
        pygame.draw.line(screen, BLACK, (center_x, center_y), (end_x, end_y), 5)
        pygame.draw.line(screen, light_color, (center_x, center_y), (end_x, end_y), 3)
    
    def can_shoot(self):
        current_time = pygame.time.get_ticks()
        return current_time - self.last_shot >= self.shot_delay
    
    def shoot(self):
        if self.can_shoot():
            self.last_shot = pygame.time.get_ticks()
            # Calcular posición del disparo según la dirección
            bullet_x = self.x + self.width // 2 - 2  # Centro del tanque
            bullet_y = self.y + self.height // 2 - 2
            
            return Bullet(bullet_x, bullet_y, self.direction)
        return None

class PlayerTank(Tank):
    """Tanque controlado por el jugador"""
    def __init__(self, x, y):
        super().__init__(x, y, DARK_GREEN)
        self.lives = 3
    
    def move(self, direction, walls, other_tanks):
        # Guardar posición actual
        old_x, old_y = self.x, self.y
        self.direction = direction
        
        # Calcular nueva posición
        if direction == Direction.UP:
            self.y -= self.speed
        elif direction == Direction.DOWN:
            self.y += self.speed
        elif direction == Direction.LEFT:
            self.x -= self.speed
        elif direction == Direction.RIGHT:
            self.x += self.speed
        
        # Verificar límites de pantalla
        if self.x < 0 or self.x + self.width > SCREEN_WIDTH:
            self.x = old_x
        if self.y < 0 or self.y + self.height > SCREEN_HEIGHT:
            self.y = old_y
        
        # Verificar colisiones con muros
        if self.check_collision_with_walls(walls):
            self.x, self.y = old_x, old_y
        
        # Verificar colisiones con otros tanques
        if self.check_collision_with_tanks(other_tanks):
            self.x, self.y = old_x, old_y
    
    def check_collision_with_walls(self, walls):
        tank_rect = self.get_rect()
        for wall in walls:
            # Solo colisionar con muros que NO están destruidos
            if not wall.destroyed and tank_rect.colliderect(wall.get_rect()):
                return True
        return False
    
    def check_collision_with_tanks(self, tanks):
        tank_rect = self.get_rect()
        for tank in tanks:
            if tank != self and tank_rect.colliderect(tank.get_rect()):
                return True
        return False

class EnemyTank(Tank):
    """Tanque enemigo (inmóvil por ahora)"""
    def __init__(self, x, y):
        super().__init__(x, y, DARK_RED)
        self.is_alive = True
    
    def update(self):
        # Por ahora los tanques enemigos están inmóviles
        # En el futuro aquí se implementará la IA
        pass

class Bullet:
    """Proyectil disparado por los tanques"""
    def __init__(self, x, y, direction):
        self.x = x
        self.y = y
        self.width = 4
        self.height = 4
        self.direction = direction
        self.speed = 5
        self.active = True
    
    def update(self):
        if not self.active:
            return
        
        # Mover según la dirección
        if self.direction == Direction.UP:
            self.y -= self.speed
        elif self.direction == Direction.DOWN:
            self.y += self.speed
        elif self.direction == Direction.LEFT:
            self.x -= self.speed
        elif self.direction == Direction.RIGHT:
            self.x += self.speed
        
        # Desactivar si sale de pantalla
        if (self.x < 0 or self.x > SCREEN_WIDTH or 
            self.y < 0 or self.y > SCREEN_HEIGHT):
            self.active = False
    
    def get_rect(self):
        return pygame.Rect(self.x, self.y, self.width, self.height)
    
    def draw(self, screen):
        if self.active:
            pygame.draw.rect(screen, BRIGHT_YELLOW, self.get_rect())
            # Agregar efecto de brillo
            pygame.draw.rect(screen, WHITE, (self.x + 1, self.y + 1, 2, 2))

class Wall:
    """Muro destructible"""
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
            # Dibujar muro con efecto 3D
            pygame.draw.rect(screen, BRICK_ORANGE, self.get_rect())
            pygame.draw.rect(screen, DARK_BROWN, self.get_rect(), 2)
            # Agregar líneas para efecto de ladrillos
            pygame.draw.line(screen, DARK_BROWN, (self.x, self.y + self.height//2), 
                           (self.x + self.width, self.y + self.height//2), 1)
            pygame.draw.line(screen, DARK_BROWN, (self.x + self.width//2, self.y), 
                           (self.x + self.width//2, self.y + self.height), 1)

class Base:
    """Base que hay que proteger"""
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
            # Dibujar base con efecto brillante
            pygame.draw.rect(screen, STEEL_BLUE, self.get_rect())
            pygame.draw.rect(screen, CYAN, self.get_rect(), 4)
            # Agregar marca de águila o estrella en el centro
            center_x = self.x + self.width // 2
            center_y = self.y + self.height // 2
            pygame.draw.circle(screen, CYAN, (center_x, center_y), 8)
            pygame.draw.circle(screen, STEEL_BLUE, (center_x, center_y), 5)

class Game:
    """Clase principal del juego"""
    def __init__(self):
        self.screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
        pygame.display.set_caption("Battle City - Un Jugador")
        self.clock = pygame.time.Clock()
        self.running = True
        
        # Inicializar objetos del juego
        self.player_tank = PlayerTank(50, 550)
        self.enemy_tanks = [
            EnemyTank(200, 50),
            EnemyTank(400, 50),
            EnemyTank(600, 50)
        ]
        self.bullets = []
        self.walls = self.create_walls()
        self.base = Base(SCREEN_WIDTH//2 - TILE_SIZE, SCREEN_HEIGHT - TILE_SIZE*2 - 10)
        
        # Estado del juego
        self.keys_pressed = set()
    
    def create_walls(self):
        """Crear el diseño del nivel con muros"""
        walls = []
        
        # Crear algunos muros de ejemplo
        # Muros horizontales
        for x in range(150, 350, TILE_SIZE):
            walls.append(Wall(x, 200))
        
        for x in range(450, 650, TILE_SIZE):
            walls.append(Wall(x, 300))
        
        # Muros verticales
        for y in range(100, 200, TILE_SIZE):
            walls.append(Wall(100, y))
        
        for y in range(350, 450, TILE_SIZE):
            walls.append(Wall(700, y))
        
        # Muros alrededor de la base
        for x in range(SCREEN_WIDTH//2 - TILE_SIZE*2, SCREEN_WIDTH//2 + TILE_SIZE*2, TILE_SIZE):
            walls.append(Wall(x, SCREEN_HEIGHT - TILE_SIZE*3))
        
        return walls
    
    def handle_events(self):
        """Manejar eventos del juego"""
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                self.running = False
            elif event.type == pygame.KEYDOWN:
                self.keys_pressed.add(event.key)
                if event.key == pygame.K_SPACE:
                    bullet = self.player_tank.shoot()
                    if bullet:
                        self.bullets.append(bullet)
            elif event.type == pygame.KEYUP:
                self.keys_pressed.discard(event.key)
    
    def update(self):
        """Actualizar el estado del juego"""
        # Mover tanque del jugador
        all_tanks = [self.player_tank] + [tank for tank in self.enemy_tanks if tank.is_alive]
        
        if pygame.K_UP in self.keys_pressed:
            self.player_tank.move(Direction.UP, self.walls, all_tanks)
        elif pygame.K_DOWN in self.keys_pressed:
            self.player_tank.move(Direction.DOWN, self.walls, all_tanks)
        elif pygame.K_LEFT in self.keys_pressed:
            self.player_tank.move(Direction.LEFT, self.walls, all_tanks)
        elif pygame.K_RIGHT in self.keys_pressed:
            self.player_tank.move(Direction.RIGHT, self.walls, all_tanks)
        
        # Actualizar tanques enemigos
        for tank in self.enemy_tanks:
            if tank.is_alive:
                tank.update()
        
        # Actualizar balas
        for bullet in self.bullets[:]:  # Copia de la lista para poder modificarla
            bullet.update()
            if not bullet.active:
                self.bullets.remove(bullet)
                continue
            
            # Verificar colisiones con muros
            for wall in self.walls:
                if not wall.destroyed and bullet.get_rect().colliderect(wall.get_rect()):
                    wall.destroyed = True
                    bullet.active = False
                    break
            
            # Verificar colisiones con tanques enemigos
            for tank in self.enemy_tanks:
                if tank.is_alive and bullet.get_rect().colliderect(tank.get_rect()):
                    tank.is_alive = False
                    bullet.active = False
                    break
            
            # Verificar colisión con la base
            if bullet.get_rect().colliderect(self.base.get_rect()):
                self.base.destroyed = True
                bullet.active = False
    
    def draw(self):
        """Dibujar todos los elementos del juego"""
        # Fondo mejorado con patrón de cuadrícula
        self.screen.fill(BACKGROUND)
        
        # Dibujar cuadrícula sutil
        grid_color = (25, 35, 45)
        for x in range(0, SCREEN_WIDTH, TILE_SIZE):
            pygame.draw.line(self.screen, grid_color, (x, 0), (x, SCREEN_HEIGHT), 1)
        for y in range(0, SCREEN_HEIGHT, TILE_SIZE):
            pygame.draw.line(self.screen, grid_color, (0, y), (SCREEN_WIDTH, y), 1)
        
        # Dibujar muros
        for wall in self.walls:
            wall.draw(self.screen)
        
        # Dibujar base
        self.base.draw(self.screen)
        
        # Dibujar tanque del jugador
        self.player_tank.draw(self.screen)
        
        # Dibujar tanques enemigos
        for tank in self.enemy_tanks:
            if tank.is_alive:
                tank.draw(self.screen)
        
        # Dibujar balas
        for bullet in self.bullets:
            bullet.draw(self.screen)
        
        # Dibujar información del juego
        font = pygame.font.Font(None, 36)
        lives_text = font.render(f"Vidas: {self.player_tank.lives}", True, WHITE)
        self.screen.blit(lives_text, (10, 10))
        
        enemies_alive = sum(1 for tank in self.enemy_tanks if tank.is_alive)
        enemies_text = font.render(f"Enemigos: {enemies_alive}", True, WHITE)
        self.screen.blit(enemies_text, (10, 50))
        
        # Instrucciones
        font_small = pygame.font.Font(None, 24)
        instructions = [
            "Controles:",
            "Flechas: Mover",
            "Espacio: Disparar"
        ]
        for i, instruction in enumerate(instructions):
            text = font_small.render(instruction, True, WHITE)
            self.screen.blit(text, (SCREEN_WIDTH - 150, 10 + i * 25))
        
        pygame.display.flip()
    
    def run(self):
        """Ciclo principal del juego"""
        while self.running:
            self.handle_events()
            self.update()
            self.draw()
            self.clock.tick(FPS)
        
        pygame.quit()
        sys.exit()

# Punto de entrada del programa
if __name__ == "__main__":
    game = Game()
    game.run()
