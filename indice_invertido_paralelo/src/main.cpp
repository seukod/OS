#include "../include/Crear_indice_inv.h"
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
    cout << "=================================================" << endl;
    cout << "       ÍNDICE INVERTIDO PARALELO                " << endl;
    cout << "=================================================" << endl;
    cout << "PID: " << getpid() << endl;

    // Verificar argumentos mínimos
    if (argc < 3) {
        cout << "Uso: " << argv[0] << " <archivo_salida> <directorio_libros>" << endl;
        cout << "Ejemplo: " << argv[0] << " indice.idx ./data/libros/" << endl;
        return 1;
    }

    string archivoSalida = argv[1];
    string directorioLibros = argv[2];

    // Configuración paralela con valores fijos
    ConfigParalelo config(4, 4);  // 4 threads, 4 libros por lote

    cout << "Archivo de salida: " << archivoSalida << endl;
    cout << "Directorio de libros: " << directorioLibros << endl;
    cout << "Configuración fija: " << config.nThreads << " threads, " << config.nLote << " libros por lote" << endl;
    cout << "=================================================" << endl;

    // Ejecutar procesamiento paralelo
    crearIndiceInvertidoParalelo(archivoSalida, directorioLibros, config);

    return 0;

}