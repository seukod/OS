#include "../../include/menus/menu_indiceinvertido.h"
#include "../../include/interfaz.h"
#include "../../include/menus/menu_principal.h"
#include "../../include/utils/input_utils.h"
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cctype>
#include <dirent.h>
#include <sys/stat.h>
#include <filesystem>

using namespace std;

struct DocumentCount {
    string doc_name;
    int count;
};

// ⭐⭐ PATH PREDEFINIDO - LIBROS EN ../data/libros ⭐⭐
const string PATH_LIBROS_PREDEFINIDO = "../data/libros";
const string CARPETA_INDICES = "../data/indices";

void crearCarpetaIndices() {
    filesystem::create_directories(CARPETA_INDICES);
}

void normalize_word(string &word) {
    string normalized;
    for (char c : word) {
        if (isalpha(static_cast<unsigned char>(c))) {
            normalized += static_cast<char>(tolower(static_cast<unsigned char>(c)));
        }
    }
    word = normalized;
}

void process_file(const string &filepath, const string &filename,
                 unordered_map<string, vector<DocumentCount>> &index) {

    ifstream file(filepath);
    if (!file.is_open()) {
        cerr << "Error abriendo: " << filepath << endl;
        return;
    }

    string word;
    int word_count = 0;
    while (file >> word) {
        normalize_word(word);
        if (word.empty()) continue;

        auto &documents = index[word];
        bool found = false;

        for (auto &doc : documents) {
            if (doc.doc_name == filename) {
                doc.count++;
                found = true;
                break;
            }
        }

        if (!found) {
            documents.push_back({filename, 1});
        }

        word_count++;
    }

    file.close();
}

bool is_valid_directory(const string &path) {
    struct stat info = {};
    if (stat(path.c_str(), &info) != 0) {
        return false;
    }
    return (info.st_mode & S_IFDIR);
}


void crearIndiceInvertido() {
    limpiarPantalla();
    cout << "=================================================" << endl;
    cout << "           Crear Índice Invertido                " << endl;
    cout << "=================================================" << endl << endl;
    cout << "PID DEL PROCESO: " << getpid() << endl;

    // ⭐⭐ 1. VALIDACIÓN DE PATH - Ingresar y verificar
    string path_ingresado;
    cout << "Ingrese el path de la carpeta con los libros: ";
    getline(cin, path_ingresado);

    // Validar que la carpeta existe

    cout << "Validando carpeta de libros..." << endl;
    path_ingresado = convertirRutaWSL(path_ingresado);
    if (!is_valid_directory(path_ingresado)) {
        cout << "[ERROR] No se puede acceder a la carpeta: " << path_ingresado << endl;
        pausarPantalla();
        return;
    }

    cout << "✓ Carpeta validada correctamente" << endl;

    // ⭐⭐ 2. SOLICITAR NOMBRE DE ARCHIVO .idx
    string nombreIndice;
    cout << "Ingrese nombre del archivo a crear (debe terminar con .idx): ";
    getline(cin, nombreIndice);

    if (nombreIndice.length() < 4 ||
        nombreIndice.substr(nombreIndice.length() - 4) != ".idx") {
        cout << "[ERROR] El nombre del archivo debe terminar con .idx" << endl;
        pausarPantalla();
        return;
    }

    // ⭐⭐ 3. CREAR CARPETA DE ÍNDICES Y RUTA COMPLETA
    crearCarpetaIndices();
    string ruta_completa = CARPETA_INDICES + "/" + nombreIndice;

    cout << "✓ Archivo se guardará en: " << ruta_completa << endl;

    // Crear índice invertido
    cout << "\n==========================================" << endl;
    cout << "INICIANDO CREACIÓN DE ÍNDICE INVERTIDO" << endl;
    cout << "==========================================" << endl;
    cout << "Carpeta de libros: " << path_ingresado << endl;
    cout << "Archivo de salida: " << ruta_completa << endl;
    cout << "==========================================" << endl << endl;

    unordered_map<string, vector<DocumentCount>> index;
    index.reserve(100000);

    // Procesar archivos
    int total_archivos = 0;
    int archivos_procesados = 0;
    DIR *dir = opendir(path_ingresado.c_str());
    if (dir != nullptr) {
        // Contar archivos primero
        struct dirent *entry;
        while ((entry = readdir(dir)) != nullptr) {
            string filename = entry->d_name;
            if (filename != "." && filename != ".." && filename[0] != '.') {
                total_archivos++;
            }
        }
        rewinddir(dir);

        cout << "Encontrados " << total_archivos << " archivos para procesar" << endl << endl;

        // Procesar archivos
        while ((entry = readdir(dir)) != nullptr) {
            string filename = entry->d_name;

            if (filename == "." || filename == ".." || filename[0] == '.') {
                continue;
            }

            string filepath = path_ingresado + "/" + filename;

            struct stat path_stat = {};
            stat(filepath.c_str(), &path_stat);
            if (S_ISREG(path_stat.st_mode)) {
                archivos_procesados++;
                cout << "[" << archivos_procesados << "/" << total_archivos << "] ";
                cout << "Procesando: " << filename << endl;
                process_file(filepath, filename, index);
            }
        }
        closedir(dir);

        cout << "\n✓ Procesamiento de archivos completado" << endl;
    } else {
        cout << "[ERROR] No se pudo abrir la carpeta: " << path_ingresado << endl;
        pausarPantalla();
        return;
    }

    // Escribir archivo en carpeta de índices
    ofstream outfile(ruta_completa);
    if (!outfile.is_open()) {
        cout << "[ERROR] No se pudo crear el archivo: " << ruta_completa << endl;
        pausarPantalla();
        return;
    }

    // Ordenar palabras alfabéticamente
    vector<string> palabras;
    palabras.reserve(index.size());
    for (const auto &entry : index) {
        palabras.push_back(entry.first);
    }
    sort(palabras.begin(), palabras.end());

    // Escribir resultados
    for (const auto &palabra : palabras) {
        const auto &documents = index[palabra];

        outfile << palabra;
        for (const auto &doc : documents) {
            outfile << ";(" << doc.doc_name << "," << doc.count << ")";
        }
        outfile << endl;
    }

    outfile.close();

    cout << "==========================================" << endl;
    cout << "PROCESO COMPLETADO EXITOSAMENTE" << endl;
    cout << "==========================================" << endl;
    cout << "✓ Índice creado: " << ruta_completa << endl;
    cout << "✓ Total de palabras únicas: " << index.size() << endl;
    cout << "✓ Archivos procesados: " << archivos_procesados << "/" << total_archivos << endl;
    cout << "==========================================" << endl;

    pausarPantalla();
}