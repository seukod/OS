#include "../../include/menus/menu_indice_inv.h"
#include "../../include/interfaz.h"
#include "../../include/utils/input_utils.h"
#include "../../include/users_auth.h"
#include "../../include/process_tools/process_manager.h"
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;

bool validarNombreArchivoIdx(const string& nombreArchivo) {
    if (nombreArchivo.length() < 5) {
        return false;
    }
    return nombreArchivo.substr(nombreArchivo.length() - 4) == ".idx";
}

bool validarDirectorioLibros(const string& pathDirectorio) {
    string ruta = pathDirectorio;

    bool esWindows =
        (ruta.find('\\') != std::string::npos) ||
        (ruta.size() > 2 && ruta[1] == ':');
    struct stat info;

    if (esWindows) {
        ruta = windowsToLinuxPath(ruta);
    }

    if (stat(ruta.c_str(), &info) != 0) {
        return false; // No existe
    }
    return (info.st_mode & S_IFDIR) != 0; // Es un directorio
}

bool crearIndiceInvertidoParalelo(const string& nombreArchivo, const string& pathCarpeta) {
    cout << "\n=================================================" << endl;
    cout << "      CREANDO ÍNDICE INVERTIDO PARALELO         " << endl;
    cout << "=================================================" << endl;
    cout << "Archivo: " << nombreArchivo << endl;
    cout << "Directorio libros: " << pathCarpeta << endl;
    cout << "=================================================" << endl;

    // Ejecutar el proceso usando la función existente (el ejecutable maneja internamente N-THREADS y N-LOTE)
    bool exito = ejecutarProcesoExterno("INDICE-INVET-PARALELO", nombreArchivo, pathCarpeta);
    if (exito) {
        cout << "exitooooooo" << endl;
    }
    else {
        cout << "no exito :c" << endl;
    }
    return exito;

}

void ejecutarMenuIndiceInv() {
    limpiarPantalla();
    cout << "=================================================" << endl;
    cout << "       CREAR ÍNDICE INVERTIDO PARALELO          " << endl;
    cout << "=================================================" << endl;

    string nombreArchivo;
    string pathCarpeta;

    // Paso 1: Ingresar y validar nombre del archivo con bucle
    do {
        cout << "Ingrese el nombre del archivo de salida (debe terminar en .idx): ";
        getline(cin, nombreArchivo);

        if (!validarNombreArchivoIdx(nombreArchivo)) {
            mostrarMensajeError("El nombre del archivo debe terminar en .idx y tener al menos 5 caracteres.");
            cout << "Ejemplo: mi_indice.idx" << endl;
        }
    } while (!validarNombreArchivoIdx(nombreArchivo));

    // Paso 2: Ingresar y validar directorio con bucle
    do {
        cout << "Ingrese la ruta del directorio de libros: ";

        getline(cin, pathCarpeta);

        if (!validarDirectorioLibros(pathCarpeta)) {
            mostrarMensajeError("El directorio especificado no existe o no es válido.");
            cout << "Ejemplo: ./data/libros/" << endl;
        }
    } while (!validarDirectorioLibros(pathCarpeta));

    cout << "\nConfiguración:" << endl;
    cout << "- Archivo: " << nombreArchivo << endl;
    cout << "- Directorio: " << pathCarpeta << endl;

    // Paso 3: Crear el índice invertido paralelo
    if (crearIndiceInvertidoParalelo(nombreArchivo, pathCarpeta)) {
        mostrarMensajeExito("Índice invertido paralelo creado exitosamente.");
        cout << "\nArchivos generados:" << endl;
        cout << "- Índice: " << nombreArchivo << endl;
        cout << "- Mapa de libros: MAPA-LIBROS.csv" << endl;
        cout << "- Ultimo Log de proceso: log_procesamiento"<< endl;
    } else {
        mostrarMensajeError("Error al crear el índice invertido paralelo.");
    }
}
