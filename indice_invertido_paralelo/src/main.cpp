#include "../include/Crear_indice_inv.h"
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
    cout << "=================================================" << endl;
    cout << "       ÍNDICE INVERTIDO PARALELO                " << endl;
    cout << "=================================================" << endl;
    cout << "PID: " << getpid() << endl;

    // Verificar argumentos
    if (argc != 3) {
        cout << "Uso: " << argv[0] << " <archivo_salida> <directorio_libros>" << endl;
        cout << "Ejemplo: " << argv[0] << " indice.idx ./data/libros/" << endl;
        return 1;
    }

    string archivoSalida = argv[1];
    string directorioLibros = argv[2];

    cout << "Archivo de salida: " << archivoSalida << endl;
    cout << "Directorio de libros: " << directorioLibros << endl;
    cout << "=================================================" << endl;

    // 1. Crear el mapa de libros primero
    vector<string> libros = escanearLibros(directorioLibros);  // Pasar la ruta como parámetro

    if (libros.empty()) {
        cout << "ERROR: No se encontraron libros en " << directorioLibros << endl;
        return 1;
    }

    // 2. Crear el archivo MAPA-LIBROS
    actualizarMapaLibros(libros);

    // 3. Aquí iría la lógica del índice invertido paralelo
    cout << "\n[INFO] Funcionalidad de índice invertido paralelo en desarrollo..." << endl;
    cout << "Mapa de libros creado exitosamente." << endl;

    return 0;

}