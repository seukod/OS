#include "../include/Crear_indice_inv.h"

using namespace std;

vector<string> escanearLibros() {
    vector<string> nombresLibros;
    string rutaLibros = "../../data/libros/";

    for (const auto& entrada: filesystem::directory_iterator(rutaLibros)) {
        if (entrada.is_regular_file()) {
            string nombreArchivo = entrada.path().filename().string();

            // Cambiar ends_with() por substr() para compatibilidad con C++17
            if (nombreArchivo.length() >= 4 &&
                nombreArchivo.substr(nombreArchivo.length() - 4) == ".txt") {
                nombresLibros.push_back(nombreArchivo);
                cout << "Libro encontrado: " << nombreArchivo << endl;
            }
        }
    }
    return nombresLibros;
}

void actualizarMapaLibros(const vector<string>& libros) {
    string rutaMapa = "../../data/MAPA-LIBROS.csv";
    ofstream archivoMapa(rutaMapa);

    if (!archivoMapa.is_open()) {
        cout << "ERROR: No se pudo crear el archivo de mapa de libros." << endl;
        return;
    }

    // Escribir encabezado
    archivoMapa << "ID,NOMBRE_LIBRO" << endl;

    // Corregir el tipo del bucle para evitar warning
    for (size_t i = 0; i < libros.size(); ++i) {
        archivoMapa << (i + 1) << "," << libros[i] << endl;
    }

    archivoMapa.close();
    cout << "Mapa de libros creado exitosamente: " << rutaMapa << endl;
    cout << "Total de libros registrados: " << libros.size() << endl;
}

/*
void crearIndiceInvertidoParalelo() {

}
*/