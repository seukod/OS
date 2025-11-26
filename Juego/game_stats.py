#!/usr/bin/env python3
"""
Generador de Estadísticas del Juego Battle City Multiplayer

Este programa analiza los logs del juego y genera 4 gráficos estadísticos:
1. Disparos por equipo en cada partida
2. Victorias por equipo (total acumulado)
3. Muertes por jugador
4. Duración promedio de partidas

Los gráficos se guardan en una carpeta configurada por variable de entorno.
"""

import os
import re
import glob
from datetime import datetime
from collections import defaultdict
import matplotlib
matplotlib.use('Agg')  # Backend sin interfaz gráfica
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches

# Configuración de la carpeta de salida
OUTPUT_DIR = os.environ.get('GAME_STATS_OUTPUT_DIR', 'stats_output')


class GameStatsAnalyzer:
    """Analizador de estadísticas del juego"""
    
    def __init__(self, logs_dir='logs'):
        self.logs_dir = logs_dir
        self.games = []  # Lista de partidas analizadas
        self.all_shots = []  # Todos los disparos
        self.all_kills = []  # Todas las muertes
        self.team_wins = defaultdict(int)  # Victorias por equipo
        
    def parse_logs(self):
        """Parsear todos los archivos de log"""
        log_files = sorted(glob.glob(os.path.join(self.logs_dir, 'game_*.log')))
        
        print(f"📂 Analizando {len(log_files)} archivos de log...")
        
        for log_file in log_files:
            self._parse_single_log(log_file)
        
        print(f"✓ Análisis completo: {len(self.games)} partidas encontradas")
        print(f"  - Total disparos: {len(self.all_shots)}")
        print(f"  - Total muertes: {len(self.all_kills)}")
        print(f"  - Victorias equipo AZUL: {self.team_wins[0]}")
        print(f"  - Victorias equipo ROJO: {self.team_wins[1]}")
    
    def _parse_single_log(self, log_file):
        """Parsear un archivo de log individual"""
        current_game = {
            'filename': os.path.basename(log_file),
            'start_time': None,
            'end_time': None,
            'duration_seconds': 0,
            'players_team_blue': 0,
            'players_team_red': 0,
            'shots_team_blue': 0,
            'shots_team_red': 0,
            'kills': [],
            'winner_team': None
        }
        
        game_started = False
        
        try:
            with open(log_file, 'r', encoding='utf-8') as f:
                for line in f:
                    # Extraer timestamp del log
                    timestamp_match = re.match(r'\[(\d{2}:\d{2}:\d{2})\]', line)
                    
                    # Inicio de partida
                    if 'GAME_START|' in line:
                        game_started = True
                        if timestamp_match:
                            current_game['start_time'] = timestamp_match.group(1)
                        
                        # Extraer jugadores por equipo
                        match = re.search(r'team_blue=(\d+)\|team_red=(\d+)', line)
                        if match:
                            current_game['players_team_blue'] = int(match.group(1))
                            current_game['players_team_red'] = int(match.group(2))
                    
                    # Disparos
                    elif 'SHOT_FIRED|' in line and game_started:
                        match = re.search(r'player_id=(\d+)\|team=(\d+)', line)
                        if match:
                            player_id = int(match.group(1))
                            team = int(match.group(2))
                            
                            if team == 0:
                                current_game['shots_team_blue'] += 1
                            else:
                                current_game['shots_team_red'] += 1
                            
                            self.all_shots.append({
                                'game': current_game['filename'],
                                'player_id': player_id,
                                'team': team
                            })
                    
                    # Muertes
                    elif 'PLAYER_KILLED|' in line and game_started:
                        match = re.search(r'killer_id=(\d+)\|killer_team=(\d+)\|victim_id=(\d+)\|victim_team=(\d+)', line)
                        if match:
                            kill_data = {
                                'game': current_game['filename'],
                                'killer_id': int(match.group(1)),
                                'killer_team': int(match.group(2)),
                                'victim_id': int(match.group(3)),
                                'victim_team': int(match.group(4))
                            }
                            current_game['kills'].append(kill_data)
                            self.all_kills.append(kill_data)
                    
                    # Fin de partida
                    elif 'GAME_END|' in line and game_started:
                        if timestamp_match:
                            current_game['end_time'] = timestamp_match.group(1)
                        
                        match = re.search(r'winner_team=(\d+)', line)
                        if match:
                            current_game['winner_team'] = int(match.group(1))
                            self.team_wins[current_game['winner_team']] += 1
                        
                        # Calcular duración
                        if current_game['start_time'] and current_game['end_time']:
                            start = datetime.strptime(current_game['start_time'], '%H:%M:%S')
                            end = datetime.strptime(current_game['end_time'], '%H:%M:%S')
                            duration = (end - start).total_seconds()
                            if duration < 0:  # El juego cruzó medianoche
                                duration += 86400
                            current_game['duration_seconds'] = duration
                        
                        game_started = False
            
            # Solo agregar partida si tiene datos válidos
            if current_game['start_time']:
                self.games.append(current_game)
        
        except Exception as e:
            print(f"⚠️  Error parseando {log_file}: {e}")
    
    def generate_graphs(self):
        """Generar todos los gráficos"""
        # Crear carpeta de salida
        os.makedirs(OUTPUT_DIR, exist_ok=True)
        print(f"\n📊 Generando gráficos en '{OUTPUT_DIR}/'...")
        
        # Configuración de estilo
        plt.style.use('seaborn-v0_8-darkgrid')
        
        # 1. Disparos por equipo por partida
        self._graph_shots_per_team()
        
        # 2. Victorias por equipo
        self._graph_team_victories()
        
        # 3. Muertes por jugador
        self._graph_deaths_per_player()
        
        # 4. Duración promedio de partidas
        self._graph_game_duration()
        
        print(f"\n✓ ¡Gráficos generados exitosamente en '{OUTPUT_DIR}/'!")
    
    def _graph_shots_per_team(self):
        """Gráfico 1: Disparos por equipo en cada partida"""
        if not self.games:
            print("⚠️  No hay datos de partidas para graficar disparos")
            return
        
        fig, ax = plt.subplots(figsize=(12, 6))
        
        game_numbers = list(range(1, len(self.games) + 1))
        shots_blue = [g['shots_team_blue'] for g in self.games]
        shots_red = [g['shots_team_red'] for g in self.games]
        
        x = range(len(game_numbers))
        width = 0.35
        
        bars1 = ax.bar([i - width/2 for i in x], shots_blue, width, 
                       label='Equipo AZUL', color='#3498db', alpha=0.8)
        bars2 = ax.bar([i + width/2 for i in x], shots_red, width,
                       label='Equipo ROJO', color='#e74c3c', alpha=0.8)
        
        ax.set_xlabel('Número de Partida', fontsize=12, fontweight='bold')
        ax.set_ylabel('Cantidad de Disparos', fontsize=12, fontweight='bold')
        ax.set_title('Disparos por Equipo en Cada Partida', fontsize=14, fontweight='bold', pad=20)
        ax.set_xticks(x)
        ax.set_xticklabels(game_numbers)
        ax.legend(loc='upper left', fontsize=10)
        ax.grid(True, alpha=0.3)
        
        # Agregar valores encima de las barras
        for bars in [bars1, bars2]:
            for bar in bars:
                height = bar.get_height()
                if height > 0:
                    ax.text(bar.get_x() + bar.get_width()/2., height,
                           f'{int(height)}',
                           ha='center', va='bottom', fontsize=9)
        
        plt.tight_layout()
        output_file = os.path.join(OUTPUT_DIR, '1_disparos_por_equipo.png')
        plt.savefig(output_file, dpi=300, bbox_inches='tight')
        plt.close()
        print(f"  ✓ Gráfico 1 guardado: {output_file}")
    
    def _graph_team_victories(self):
        """Gráfico 2: Victorias por equipo (acumulado)"""
        if not self.team_wins:
            print("⚠️  No hay datos de victorias para graficar")
            return
        
        fig, ax = plt.subplots(figsize=(10, 6))
        
        teams = ['Equipo AZUL', 'Equipo ROJO']
        victories = [self.team_wins[0], self.team_wins[1]]
        colors = ['#3498db', '#e74c3c']
        
        bars = ax.bar(teams, victories, color=colors, alpha=0.8, edgecolor='black', linewidth=1.5)
        
        ax.set_ylabel('Cantidad de Victorias', fontsize=12, fontweight='bold')
        ax.set_title('Total de Victorias por Equipo', fontsize=14, fontweight='bold', pad=20)
        ax.grid(True, axis='y', alpha=0.3)
        
        # Agregar valores encima de las barras
        for bar in bars:
            height = bar.get_height()
            ax.text(bar.get_x() + bar.get_width()/2., height,
                   f'{int(height)}',
                   ha='center', va='bottom', fontsize=14, fontweight='bold')
        
        # Agregar porcentajes
        total = sum(victories)
        if total > 0:
            for i, bar in enumerate(bars):
                height = bar.get_height()
                percentage = (height / total) * 100
                ax.text(bar.get_x() + bar.get_width()/2., height * 0.5,
                       f'{percentage:.1f}%',
                       ha='center', va='center', fontsize=12, 
                       fontweight='bold', color='white')
        
        plt.tight_layout()
        output_file = os.path.join(OUTPUT_DIR, '2_victorias_por_equipo.png')
        plt.savefig(output_file, dpi=300, bbox_inches='tight')
        plt.close()
        print(f"  ✓ Gráfico 2 guardado: {output_file}")
    
    def _graph_deaths_per_player(self):
        """Gráfico 3: Muertes por jugador"""
        if not self.all_kills:
            print("⚠️  No hay datos de muertes para graficar")
            return
        
        # Contar muertes por jugador
        deaths_count = defaultdict(int)
        for kill in self.all_kills:
            victim_id = kill['victim_id']
            deaths_count[victim_id] += 1
        
        if not deaths_count:
            print("⚠️  No hay datos válidos de muertes")
            return
        
        fig, ax = plt.subplots(figsize=(10, 6))
        
        # Ordenar jugadores por ID
        players = sorted(deaths_count.keys())
        player_labels = [f'Jugador {pid + 1}' for pid in players]
        death_counts = [deaths_count[pid] for pid in players]
        
        # Colores según el jugador (alternando)
        colors = ['#3498db' if pid % 2 == 0 else '#e74c3c' for pid in players]
        
        bars = ax.bar(player_labels, death_counts, color=colors, alpha=0.8, 
                     edgecolor='black', linewidth=1.5)
        
        ax.set_ylabel('Cantidad de Muertes', fontsize=12, fontweight='bold')
        ax.set_title('Muertes por Jugador (Todas las Partidas)', fontsize=14, fontweight='bold', pad=20)
        ax.grid(True, axis='y', alpha=0.3)
        
        # Agregar valores encima de las barras
        for bar in bars:
            height = bar.get_height()
            ax.text(bar.get_x() + bar.get_width()/2., height,
                   f'{int(height)}',
                   ha='center', va='bottom', fontsize=12, fontweight='bold')
        
        # Resaltar al jugador que más murió
        if death_counts:
            max_deaths = max(death_counts)
            max_idx = death_counts.index(max_deaths)
            bars[max_idx].set_edgecolor('gold')
            bars[max_idx].set_linewidth(3)
            
            # Agregar anotación
            ax.annotate('¡Más muertes! 💀', 
                       xy=(max_idx, max_deaths),
                       xytext=(max_idx, max_deaths + 1),
                       ha='center',
                       fontsize=11,
                       fontweight='bold',
                       color='#e67e22',
                       arrowprops=dict(arrowstyle='->', color='#e67e22', lw=2))
        
        plt.tight_layout()
        output_file = os.path.join(OUTPUT_DIR, '3_muertes_por_jugador.png')
        plt.savefig(output_file, dpi=300, bbox_inches='tight')
        plt.close()
        print(f"  ✓ Gráfico 3 guardado: {output_file}")
    
    def _graph_game_duration(self):
        """Gráfico 4: Duración promedio de partidas"""
        # Filtrar juegos con duración válida
        valid_games = [g for g in self.games if g['duration_seconds'] > 0]
        
        if not valid_games:
            print("⚠️  No hay datos de duración para graficar")
            return
        
        fig, ax = plt.subplots(figsize=(12, 6))
        
        game_numbers = list(range(1, len(valid_games) + 1))
        durations_minutes = [g['duration_seconds'] / 60 for g in valid_games]
        
        # Línea de duración
        line = ax.plot(game_numbers, durations_minutes, marker='o', 
                       linewidth=2, markersize=8, color='#9b59b6', 
                       label='Duración', alpha=0.8)
        
        # Línea promedio
        avg_duration = sum(durations_minutes) / len(durations_minutes)
        ax.axhline(y=avg_duration, color='#e74c3c', linestyle='--', 
                  linewidth=2, label=f'Promedio: {avg_duration:.1f} min', alpha=0.7)
        
        ax.set_xlabel('Número de Partida', fontsize=12, fontweight='bold')
        ax.set_ylabel('Duración (minutos)', fontsize=12, fontweight='bold')
        ax.set_title('Duración de Partidas y Promedio', fontsize=14, fontweight='bold', pad=20)
        ax.set_xticks(game_numbers)
        ax.legend(loc='upper right', fontsize=10)
        ax.grid(True, alpha=0.3)
        
        # Agregar valores en algunos puntos
        for i, (x, y) in enumerate(zip(game_numbers, durations_minutes)):
            if i % max(1, len(game_numbers) // 5) == 0:  # Mostrar cada N puntos
                ax.text(x, y + 0.2, f'{y:.1f}', 
                       ha='center', va='bottom', fontsize=9)
        
        # Sombreado del área bajo la curva
        ax.fill_between(game_numbers, durations_minutes, alpha=0.2, color='#9b59b6')
        
        plt.tight_layout()
        output_file = os.path.join(OUTPUT_DIR, '4_duracion_partidas.png')
        plt.savefig(output_file, dpi=300, bbox_inches='tight')
        plt.close()
        print(f"  ✓ Gráfico 4 guardado: {output_file}")
    
    def print_summary(self):
        """Imprimir resumen de estadísticas"""
        print("\n" + "="*60)
        print("📈 RESUMEN DE ESTADÍSTICAS")
        print("="*60)
        
        print(f"\n🎮 Total de partidas analizadas: {len(self.games)}")
        
        if self.games:
            print(f"\n💥 Disparos:")
            total_shots = sum(g['shots_team_blue'] + g['shots_team_red'] for g in self.games)
            shots_blue = sum(g['shots_team_blue'] for g in self.games)
            shots_red = sum(g['shots_team_red'] for g in self.games)
            print(f"   - Total: {total_shots}")
            print(f"   - Equipo AZUL: {shots_blue} ({shots_blue/total_shots*100:.1f}%)" if total_shots > 0 else "   - Equipo AZUL: 0")
            print(f"   - Equipo ROJO: {shots_red} ({shots_red/total_shots*100:.1f}%)" if total_shots > 0 else "   - Equipo ROJO: 0")
        
        if self.team_wins:
            print(f"\n🏆 Victorias:")
            total_wins = sum(self.team_wins.values())
            print(f"   - Equipo AZUL: {self.team_wins[0]} ({self.team_wins[0]/total_wins*100:.1f}%)" if total_wins > 0 else "   - Equipo AZUL: 0")
            print(f"   - Equipo ROJO: {self.team_wins[1]} ({self.team_wins[1]/total_wins*100:.1f}%)" if total_wins > 0 else "   - Equipo ROJO: 0")
        
        if self.all_kills:
            print(f"\n💀 Muertes:")
            deaths_count = defaultdict(int)
            for kill in self.all_kills:
                deaths_count[kill['victim_id']] += 1
            
            print(f"   - Total de eliminaciones: {len(self.all_kills)}")
            for pid in sorted(deaths_count.keys()):
                print(f"   - Jugador {pid + 1}: {deaths_count[pid]} muertes")
            
            if deaths_count:
                most_deaths_player = max(deaths_count.items(), key=lambda x: x[1])
                print(f"\n   ⚠️  Jugador que más murió: Jugador {most_deaths_player[0] + 1} ({most_deaths_player[1]} veces)")
        
        valid_games = [g for g in self.games if g['duration_seconds'] > 0]
        if valid_games:
            print(f"\n⏱️  Duración de partidas:")
            durations = [g['duration_seconds'] / 60 for g in valid_games]
            print(f"   - Promedio: {sum(durations)/len(durations):.2f} minutos")
            print(f"   - Más corta: {min(durations):.2f} minutos")
            print(f"   - Más larga: {max(durations):.2f} minutos")
        
        print("\n" + "="*60)


def main():
    """Función principal"""
    print("="*60)
    print("🎮 GENERADOR DE ESTADÍSTICAS - BATTLE CITY MULTIPLAYER")
    print("="*60)
    print(f"📁 Carpeta de salida: {OUTPUT_DIR}")
    print(f"   (Configurable con variable de entorno GAME_STATS_OUTPUT_DIR)")
    print("="*60)
    
    # Crear analizador
    analyzer = GameStatsAnalyzer()
    
    # Parsear logs
    analyzer.parse_logs()
    
    # Imprimir resumen
    analyzer.print_summary()
    
    # Generar gráficos
    analyzer.generate_graphs()
    
    print("\n✅ ¡Proceso completado!")


if __name__ == "__main__":
    main()
