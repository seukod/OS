#include "../../include/menus/menu_indice.h"
#include "../../include/interfaz.h"
#include "../../include/utils/input_utils.h"
#include "../../include/users_auth.h"
#include "../../include/process_tools/process_manager.h"
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;

bool validarNombreArchivo(const string& nombreArchivo) {
    if (nombreArchivo.length() < 5) {
        return false;
    }
    return nombreArchivo.substr(nombreArchivo.length() - 4) == ".idx";
}

bool validarDirectorio(const string& pathDirectorio) {
    struct stat info;
    if (stat(pathDirectorio.c_str(), &info) != 0) {
        return false; // No existe
    }
    return (info.st_mode & S_IFDIR) != 0; // Es un directorio
}

bool crearIndiceInvertido(const string& nombreArchivo, const string& pathCarpeta) {
    cout << "\n=================================================" << endl;
    cout << "           CREANDO ÍNDICE INVERTIDO              " << endl;
    cout << "=================================================" << endl;
    cout << "Archivo: " << nombreArchivo << endl;
    cout << "Directorio libros: " << pathCarpeta << endl;
    cout << "=================================================" << endl;

    // La función ejecutarProcesoExterno ya imprime sus propios mensajes
    bool exito = ejecutarProcesoExterno("CREATE_INDEX", nombreArchivo, pathCarpeta);

    return exito;
}

void ejecutarMenuIndice() {
    limpiarPantalla();
    cout << "=================================================" << endl;
    cout << "           CREAR ÍNDICE INVERTIDO                " << endl;
    cout << "=================================================" << endl;
    
    string nombreArchivo;
    string pathCarpeta;
    
    // Paso 1: Ingresar y validar nombre del archivo con bucle
    do {
        cout << "\nIngrese el nombre del archivo a crear (debe terminar en .idx): ";
        getline(cin, nombreArchivo);
        
        if (nombreArchivo.empty()) {
            cout << "[ERROR] El nombre no puede estar vacío." << endl;
        } else if (!validarNombreArchivo(nombreArchivo)) {
            cout << "[ERROR] El archivo debe tener extensión .idx" << endl;
        } else {
            break;
        }
    } while (true);
    
    // Paso 2: Ingresar y validar path de la carpeta con bucle
    do {
        cout << "\nIngrese el path de la carpeta donde están los libros: ";
        getline(cin, pathCarpeta);
        
        if (pathCarpeta.empty()) {
            cout << "[ERROR] El path no puede estar vacío." << endl;
        } else if (!validarDirectorio(pathCarpeta)) {
            cout << "[ERROR] El directorio no existe o no es válido." << endl;
            cout << "Verifique que el path sea correcto." << endl;
        } else {
            break;
        }
    } while (true);
    
    // Paso 3: Confirmar antes de ejecutar con validación
    cout << "\n=================================================" << endl;
    cout << "RESUMEN:" << endl;
    cout << "Archivo a crear: " << nombreArchivo << endl;
    cout << "Directorio de libros: " << pathCarpeta << endl;
    cout << "=================================================" << endl;
    
    char confirmacion;
    do {
        cout << "¿Desea proceder? (s/n): ";
        cin >> confirmacion;
        cin.ignore(); // Limpiar buffer

        if (confirmacion == 's' || confirmacion == 'S' || confirmacion == 'n' || confirmacion == 'N') {
            break;
        } else {
            cout << "[ERROR] Por favor ingrese 's' para sí o 'n' para no." << endl;
        }
    } while (true);

    if (confirmacion == 's' || confirmacion == 'S') {
        // Paso 4: Ejecutar programa externo
        crearIndiceInvertido(nombreArchivo, pathCarpeta);
        // El proceso hijo ya muestra los mensajes de resultado
    } else {
        cout << "\nOperación cancelada por el usuario." << endl;
    }
}
