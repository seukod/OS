#include "../../include/menus/menu_matriz.h"
#include "../../include/interfaz.h"
#include "../../include/utils/input_utils.h"
#include "../../include/process_tools/process_manager.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <limits>

using namespace std;

bool validarMatriz(const string& archivo) {
    ifstream file(archivo);
    if (!file.good()) {
        return false;
    }
    file.close();
    return true;
}

string convertirRutaAbsoluta(const string& ruta) {
    if (ruta[0] == '/') {
        return ruta; // Ya es absoluta
    }

    char* cwd = getcwd(nullptr, 0);
    string rutaAbsoluta = string(cwd) + "/../" + ruta;
    free(cwd);
    return rutaAbsoluta;
}

void ejecutarMenuMatriz() {
    limpiarPantalla();
    cout << "========================================" << endl;
    cout << "    CONFIGURACIÓN DE MATRICES" << endl;
    cout << "========================================" << endl;

    // i) Ingresar el N de la matriz con validación
    int n;
    do {
        cout << "Ingrese el tamaño N de las matrices (NxN): ";
        if (cin >> n) {
            if (n > 0 && n <= 10) { // Validar rango razonable
                cin.ignore(); // Limpiar buffer
                break;
            } else {
                cout << "[ERROR] El tamaño debe ser un número entre 1 y 10." << endl;
            }
        } else {
            cout << "[ERROR] Por favor ingrese un número válido." << endl;
            cin.clear(); // Limpiar el estado de error
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Limpiar buffer
        }
    } while (true);

    // ii) Ingresar los paths de las dos matrices con validación
    string archivo1, archivo2;

    // Validar primer archivo
    do {
        cout << "Ingrese el path del primer archivo de matriz: ";
        getline(cin, archivo1);

        if (archivo1.empty()) {
            cout << "[ERROR] El path no puede estar vacío." << endl;
        } else {
            string archivoAbsoluto1 = convertirRutaAbsoluta(archivo1);
            if (validarMatriz(archivoAbsoluto1)) {
                break;
            } else {
                cout << "[ERROR] No se encuentra el archivo: " << archivo1 << endl;
                cout << "Verifique que el path sea correcto." << endl;
            }
        }
    } while (true);

    // Validar segundo archivo
    do {
        cout << "Ingrese el path del segundo archivo de matriz: ";
        getline(cin, archivo2);

        if (archivo2.empty()) {
            cout << "[ERROR] El path no puede estar vacío." << endl;
        } else {
            string archivoAbsoluto2 = convertirRutaAbsoluta(archivo2);
            if (validarMatriz(archivoAbsoluto2)) {
                break;
            } else {
                cout << "[ERROR] No se encuentra el archivo: " << archivo2 << endl;
                cout << "Verifique que el path sea correcto." << endl;
            }
        }
    } while (true);

    // Mostrar resumen de configuración
    cout << "\n--- RESUMEN DE CONFIGURACIÓN ---" << endl;
    cout << "Tamaño de matrices: " << n << "x" << n << endl;
    cout << "Archivo matriz A: " << archivo1 << endl;
    cout << "Archivo matriz B: " << archivo2 << endl;
    cout << "================================" << endl;

    // iii) Lanzar la ejecución o volver con validación
    int opcion;
    do {
        cout << "\n1. Lanzar ejecución" << endl;
        cout << "2. Volver al menú principal" << endl;
        cout << "Seleccione una opción: ";

        if (cin >> opcion) {
            cin.ignore(); // Limpiar buffer
            if (opcion == 1 || opcion == 2) {
                break;
            } else {
                cout << "[ERROR] Seleccione 1 o 2." << endl;
            }
        } else {
            cout << "[ERROR] Por favor ingrese un número válido (1 o 2)." << endl;
            cin.clear(); // Limpiar el estado de error
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Limpiar buffer
        }
    } while (true);

    if (opcion == 1) {
        // Convertir rutas a absolutas para la ejecución
        string archivoAbsoluto1 = convertirRutaAbsoluta(archivo1);
        string archivoAbsoluto2 = convertirRutaAbsoluta(archivo2);

        cout << "\nValidando archivos..." << endl;
        cout << "Archivo 1: " << archivoAbsoluto1 << endl;
        cout << "Archivo 2: " << archivoAbsoluto2 << endl;
        cout << "Archivos validados correctamente." << endl;

        // La función ejecutarProcesoExterno ya imprime sus propios mensajes
        bool exito = ejecutarProcesoExterno("MULTI_M", archivoAbsoluto1, archivoAbsoluto2);

        if (exito) {
            mostrarMensajeExito("Multiplicación completada exitosamente.");
        } else {
            mostrarMensajeError("Error en la ejecución del multiplicador.");
        }
    } else {
        cout << "Volviendo al menú principal..." << endl;
    }

    pausarPantalla();
}
