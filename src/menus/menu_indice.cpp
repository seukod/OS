#include "../../include/menus/menu_indice.h"
#include "../../include/interfaz.h"
#include "../../include/users_auth.h"

#include <iostream>
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
using namespace std;
namespace fs = std::filesystem;


// Función para manejar la lógica del archivo
string nombreArchivo() {
    string archivo;
    int opcion;

    // Obtener carpeta desde variable de entorno o .env
    string carpeta = leerVariableEnv("Revisar", ".env");
    if (carpeta.empty()) {
        cout << "[ERROR] No se encontró la variable 'Revisar' en el entorno ni en .env." << endl;
        return "";
    }
    if (carpeta.back() != '/' && carpeta.back() != '\\') carpeta += '/'; // asegura que termine con /

    while (true) {
        cout << "Ingrese el nombre del archivo a crear (sin extensión, se agregará .idx automáticamente): ";
        cin >> archivo;

        // Forzar extensión .idx
        if (archivo.size() < 4 || archivo.substr(archivo.size() - 4) != ".idx") {
            archivo += ".idx";
        }

        string rutaCompleta = carpeta + archivo;

        if (fs::exists(rutaCompleta)) {
            cout << "El archivo '" << archivo << "' ya existe." << endl;
            cout << "¿Quieres editar el archivo? (1 = Sí, 0 = No): ";
            cin >> opcion;

            if (opcion == 1) {
                cout << "Se editará el archivo existente '" << archivo << "'." << endl;
                return rutaCompleta;
            } else {
                limpiarPantalla();  // limpiar pantalla si no quiere editar
                cout << "Ingrese otro nombre de archivo." << endl;
            }
        } else {
            cout << "El archivo '" << archivo << "' no existe. Se creará uno nuevo." << endl;
            ofstream nuevo(rutaCompleta);
            if (!nuevo) {
                cout << "[ERROR] No se pudo crear el archivo en la ruta: " << rutaCompleta << endl;
                return "";
            }
            nuevo.close();
            return rutaCompleta;
        }
    }
}

string pedirPathValido() {
    string path;
    while (true) {
        cout << "Ingrese el path de la carpeta con los libros: ";
        cin >> path;

        if (fs::exists(path) && fs::is_directory(path)) {
            return path;  // Path válido
        } else {
            cout << "[ERROR] La carpeta no existe. Intente nuevamente." << endl;
        }
    }
}


// Función principal
void ejecutarMenuIndiceInvertido() {
    string nombreArchivoRuta;
    string pathCarpeta;
    int confirmar;

    limpiarPantalla();
    cout << "=================================================" << endl;
    cout << "         CREAR ÍNDICE INVERTIDO                  " << endl;
    cout << "=================================================" << endl;

    // Pregunta 1 (usa la función nombreArchivo)
    nombreArchivoRuta = nombreArchivo();
    if (nombreArchivoRuta.empty()) {
        pausarPantalla();
        return; // Si el usuario canceló
    }

    // Pregunta 2
    pathCarpeta = pedirPathValido();

    // Pregunta 3 (confirmación)
    cout << "¿Está seguro de ejecutar el programa externo? (1 = Aceptar, 0 = Cancelar): ";
    cin >> confirmar;

    if (confirmar == 1) {
        cout << "Ejecutando programa externo con archivo '" << nombreArchivoRuta
             << "' y carpeta '" << pathCarpeta << "'..." << endl;
    } else {
        cout << "Ejecución cancelada. Volviendo al menú..." << endl;
    }

    pausarPantalla();
}
