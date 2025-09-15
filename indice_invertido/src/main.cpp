#include "../include/crear_indice.h"
#include <iostream>
#include <filesystem>

using namespace std;
namespace fs = filesystem;

int main(int argc, char* argv[]) {
    // Verificar argumentos
    if (argc != 3) {
        cout << "Uso: " << argv[0] << " <nombre-archivo.idx> <path-carpeta-libros>" << endl;
        cout << "Ejemplo: " << argv[0] << " mi_indice.idx /home/user/libros" << endl;
        return 1;
    }
    
    string nombreArchivo = argv[1];
    string pathCarpeta = argv[2];
    
    // Validar que el archivo tenga extensión .idx
    if (nombreArchivo.length() < 5 || 
        nombreArchivo.substr(nombreArchivo.length() - 4) != ".idx") {
        cout << "ERROR: El archivo debe tener extensión .idx" << endl;
        return 2;
    }
    
    // Construir la ruta completa para guardar en OS/
    // El programa se ejecuta desde OS/build
    string rutaCompleta = "../" + nombreArchivo;
    
    try {
        // Ejecutar el proceso de creación usando la función
        if (crearIndice(rutaCompleta, pathCarpeta)) {
            cout << "\n¡Índice invertido creado exitosamente!" << endl;
            return 0;
        } else {
            cout << "\nERROR: No se pudo crear el índice invertido." << endl;
            return 3;
        }
    } catch (const exception& e) {
        cout << "ERROR: " << e.what() << endl;
        return 4;
    } catch (...) {
        cout << "ERROR: Error en la creación del índice." << endl;
        return 5;
    }
}
