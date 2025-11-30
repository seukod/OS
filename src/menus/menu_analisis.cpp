#include "../../include/menus/menu_analisis.h"
#include "../../include/utils/input_utils.h"
#include "../../include/interfaz.h"
#include "../../include/process_tools/process_manager.h"

#include <iostream>
#include <vector>
#include <cstdlib>     // getenv
#include <sstream>

using namespace std;

void mostrarMenuAnalisis() {
    limpiarPantalla();
    cout << "==================================" << endl;
    cout << "   ANALISIS DE RENDIMIENTO T2A    " << endl;
    cout << "==================================" << endl;
    cout << "1. Configurar arreglo CANT_THREADS" << endl;
    cout << "2. Ejecutar analisis" << endl;
    cout << "0. Volver" << endl;
    cout << "==================================" << endl;
}

static vector<int> CANT_THREADS; // arreglo global interno

void configurarArregloThreads() {
    int cantidad = obtenerEntero("Ingrese la cantidad de valores para CANT_THREADS: ");
    CANT_THREADS.clear();
    CANT_THREADS.resize(cantidad);

    for (int i = 0; i < cantidad; i++) {
        string msg = "Ingrese valor para CANT_THREADS[" + to_string(i) + "]: ";
        CANT_THREADS[i] = obtenerEntero(msg);
    }

    mostrarMensajeExito("Arreglo configurado correctamente.");
    pausarPantalla();
}

void ejecutarAnalisisRendimiento() {
    if (CANT_THREADS.empty()) {
        mostrarMensajeError("Debe configurar el arreglo CANT_THREADS antes de ejecutar el análisis.");
        pausarPantalla();
        return;
    }

    string log_path = getenv("ANALISIS_LOG_PATH") ? getenv("ANALISIS_LOG_PATH") : "logs/analisis_threads.log";

    cout << "📌 Guardando resultados en: " << log_path << endl;

    for (int t : CANT_THREADS) {
        cout << "Ejecutando analisis con " << t << " threads..." << endl;

        // Llamar al programa externo que genera el índice con threads
        std::stringstream cmd;
        cmd << "./analisis_threads " << t << " >> " << log_path;

        system(cmd.str().c_str());
    }

    cout << endl << "✔️ Ejecuciones completadas." << endl;
    cout << "Generando gráfico con Python..." << endl;

    // Llamar al script de python
    system("python3 scripts/grafico_rendimiento.py");

    mostrarMensajeExito("Analisis completado. Revise la carpeta de resultados.");
    pausarPantalla();
}

void ejecutarMenuAnalisis() {
    int opcion;

    do {
        mostrarMenuAnalisis();
        opcion = obtenerEntero("Seleccione una opcion: ");

        switch (opcion) {
            case 1:
                configurarArregloThreads();
                break;
            case 2:
                ejecutarAnalisisRendimiento();
                break;
            case 0:
                return;
            default:
                mostrarMensajeError("Opcion invalida.");
                pausarPantalla();
                break;
        }
    } while (opcion != 0);
}
