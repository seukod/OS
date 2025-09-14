//
// Created by Francisco Contreras on 13-09-25.
//
#include "../include/menu_matriz.h"
#include "../../include/interfaz.h"
#include "../../include/utils/input_utils.h"
#include "../../include/users_auth.h"
#include "../../include/process_tools/process_manager.h"
#include <unistd.h>
#include <fstream>
#include <sys/wait.h>

void menuMatriz() {
    limpiarPantalla();
    mostrarTitulo("MENÚ MULTI MATRICES NxN");
    cout << "Funcionalidad para multiplicar matrices cuadradas NxN desde archivos." << endl;
    mostrarSeparador();

    const string multiplicadorPath = leerVariableEnv("MULTI_M",".env");

    if (multiplicadorPath.empty()) {
        mostrarMensajeError("Variable MULTI_M no encontrada en .env");
        pausarPantalla();
        return;
    }

    // Verificar que el ejecutable existe
    if (access(multiplicadorPath.c_str(), F_OK) != 0) {
        mostrarMensajeError("El ejecutable no existe: " + multiplicadorPath);
        pausarPantalla();
        return;
    }

    // Interfaz para ingresar datos
    cout << "Ingrese el tamaño N de las matrices (NxN): ";
    int n = leerOpcion();

    string archivo1, archivo2;
    cout << "Ingrese el path del primer archivo de matriz: ";
    cin.ignore();
    getline(cin, archivo1);
    cout << "Ingrese el path del segundo archivo de matriz: ";
    getline(cin, archivo2);

    // Validar existencia de archivos antes de llamar al multiplicador
    if (!validarExistenciaArchivo(archivo1)) {
        mostrarMensajeError("No se encuentra el primer archivo: " + archivo1);
        pausarPantalla();
        return;
    }

    if (!validarExistenciaArchivo(archivo2)) {
        mostrarMensajeError("No se encuentra el segundo archivo: " + archivo2);
        pausarPantalla();
        return;
    }

    // Mostrar opción de lanzar ejecución o volver
    cout << "\nArchivos validados correctamente." << endl;
    cout << "Archivo 1: " << archivo1 << endl;
    cout << "Archivo 2: " << archivo2 << endl;
    cout << "Tamaño: " << n << "x" << n << endl;
    cout << "\n1. Lanzar ejecución" << endl;
    cout << "2. Volver al menú principal" << endl;
    cout << "Seleccione una opción: ";

    int opcion = leerOpcion();

    if (opcion == 1) {
        ejecutarMultiplicador(multiplicadorPath, archivo1, archivo2);
    } else {
        cout << "Volviendo al menú principal..." << endl;
    }

    pausarPantalla();
}

bool validarExistenciaArchivo(const string& archivo) {
    ifstream file(archivo);
    if (!file.good()) {
        return false;
    }
    file.close();
    return true;
}

void ejecutarMultiplicador(const string& multiplicadorPath, const string& archivo1, const string& archivo2) {
    // Usar la función del process_manager que maneja el fork y execl
    bool exito = ejecutarMultiplicadorMatrices(archivo1, archivo2);
    
    if (!exito) {
        pausarPantalla();
    }
}
