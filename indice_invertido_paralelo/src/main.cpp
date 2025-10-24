#include "../include/Crear_indice_inv.h"
#include <thread>



using namespace std;


int main() {
    vector<string> libros = escanearLibros();

    if (libros.empty()) {
        cout << "No se encontraron libros." << endl;
        return 1;
    }

    actualizarMapaLibros(libros);
    return 0;

}