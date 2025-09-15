#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
using namespace std;
namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cout << "Uso: programa_externo <archivo.idx> <carpeta_libros>" << endl;
        return 1;
    }

    string archivoIdx = argv[1];
    string carpetaLibros = argv[2];

    if (!fs::exists(carpetaLibros) || !fs::is_directory(carpetaLibros)) {
        cout << "[ERROR] La carpeta no existe: " << carpetaLibros << endl;
        return 1;
    }

    ofstream idx(archivoIdx);
    if (!idx) {
        cout << "[ERROR] No se pudo abrir el archivo: " << archivoIdx << endl;
        return 1;
    }

    cout << "Listando libros en: " << carpetaLibros << endl;

    for (const auto& entry : fs::directory_iterator(carpetaLibros)) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            string nombreLibro = entry.path().filename().string();
            idx << nombreLibro << endl;
            cout << "Agregado: " << nombreLibro << endl;
        }
    }

    idx.close();
    cout << "Se completó la creación del índice en " << archivoIdx << endl;
    return 0;
}
