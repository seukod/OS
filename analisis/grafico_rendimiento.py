import os
import matplotlib.pyplot as plt

# ===============================
# VARIABLES DE ENTORNO
# ===============================
log_path = os.getenv("ANALISIS_LOG_PATH", "logs/analisis_threads.log")
output_dir = os.getenv("ANALISIS_IMG_PATH", "graficos")

# Crear carpeta si no existe
os.makedirs(output_dir, exist_ok=True)

threads = []
tiempos = []

# ===============================
# LEER LOG
# ===============================
try:
    with open(log_path, "r") as f:
        for linea in f:
            linea = linea.strip()
            if not linea:
                continue
            t, tiempo = linea.split()
            threads.append(int(t))
            tiempos.append(int(tiempo))
except FileNotFoundError:
    print("ERROR: No se encontró el log:", log_path)
    exit(1)

# ===============================
# GRAFICAR
# ===============================
plt.figure(figsize=(8, 5))
plt.plot(threads, tiempos, marker="o", linestyle="-")

plt.title("Rendimiento de Índice Invertido según Threads")
plt.xlabel("Cantidad de Threads")
plt.ylabel("Tiempo (ms)")
plt.grid(True)

# ===============================
# GUARDAR IMAGEN
# ===============================
output_file = os.path.join(output_dir, "grafico_rendimiento.png")
plt.savefig(output_file)

print("Gráfico creado en:", output_file)
