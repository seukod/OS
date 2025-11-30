#include <iostream>
#include <chrono>
#include <vector>
#include <string>
#include <cstdlib>
#include <fstream>

using namespace std;
using namespace std::chrono;

// ===================
// CONFIGURACIÓN
// ===================

// 🔥 Ruta REAL del ejecutable que genera el índice invertido en tu proyecto
//   *** EDITA SOLO ESTA LÍNEA si el ejecutable está en otro lado ***
static const string PROGRAMA_INDICE =
    "/home/javier/Escritorio/github/OS/build/generar_indice_inv";


// ===================
// FUNCIÓN PRINCIPAL
// ===================
int main(int argc, char* argv[]) 
{
    if (argc != 2) {
        cerr << "Uso: analisis_threads <num_threads>" << endl;
        return 1;
    }

    int threads = atoi(argv[1]);

    // Para guardar internamente los tiempos por si quieres usarlos luego
    vector<long> tiempos_ejecucion;

    // Tomar tiempo inicial
    auto inicio = high_resolution_clock::now();

    // ------------------------
    // Ejecutar el índice con N threads
    // ------------------------
    string cmd = PROGRAMA_INDICE + " " + to_string(threads);
    int result = system(cmd.c_str());

    if (result != 0) {
        cerr << "ERROR: no se pudo ejecutar el generador de indice invertido." << endl;
        return 1;
    }

    // Tomar tiempo final
    auto fin = high_resolution_clock::now();

    // Calcular duración en milisegundos
    long duracion_ms = duration_cast<milliseconds>(fin - inicio).count();

    // Guardar en arreglo interno
    tiempos_ejecucion.push_back(duracion_ms);

    // ==============================
    // ESCRIBIR EN EL LOG
    // ==============================
    const char* env_log = getenv("ANALISIS_LOG_PATH");
    string log_path = env_log ? env_log : "logs/analisis_threads.log";

    ofstream log_file(log_path, ios::app);

    if (!log_file.is_open()) {
        cerr << "Error al abrir el log en: " << log_path << endl;
        return 1;
    }

    // FORMATO QUE USA PYTHON PARA GRAFICAR:
    // threads tiempo_ms
    log_file << threads << " " << duracion_ms << endl;

    log_file.close();

    // Mostrar en consola para depuración
    cout << "[analisis_threads] "
         << "threads=" << threads
         << " tiempo=" << duracion_ms << "ms" << endl;

    return 0;
}
