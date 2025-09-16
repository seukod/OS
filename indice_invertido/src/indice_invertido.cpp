#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <dirent.h>
#include <string>
#include <sys/stat.h>

using namespace std;

struct DocumentCount {
    string doc_name;
    int count;
};

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
        if (word_count % 1000 == 0) {
            cout << "Procesadas " << word_count << " palabras en " << filename << endl;
        }
    }

    file.close();
    cout << "Finalizado: " << filename << " - " << word_count << " palabras procesadas" << endl;
}

bool is_valid_directory(const string &path) {
    struct stat info = {};
    if (stat(path.c_str(), &info) != 0) {
        return false;
    }
    return (info.st_mode & S_IFDIR);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cout << "Uso: " << argv[0] << " <nombre-archivo.idx> <path-carpeta-libros>" << endl;
        cout << "Ejemplo: " << argv[0] << " mi_indice.idx ./libros" << endl;
        return 1;
    }

    string output_file = argv[1];
    string folder_path = argv[2];

    if (output_file.length() < 4 || output_file.substr(output_file.length() - 4) != ".idx") {
        cout << "Error: El archivo debe tener extensión .idx" << endl;
        return 1;
    }

    cout << "Validando ruta de la carpeta..." << endl;

    if (!is_valid_directory(folder_path)) {
        cout << "✗ Error: No se puede acceder a la carpeta: " << folder_path << endl;
        cout << "Por favor, verifique la ruta e intente nuevamente." << endl;
        return 1;
    }

    cout << "✓ Ruta de carpeta validada correctamente: " << folder_path << endl;

    cout << "\n==========================================" << endl;
    cout << "INICIANDO CREACIÓN DE ÍNDICE INVERTIDO" << endl;
    cout << "==========================================" << endl;
    cout << "Carpeta de libros: " << folder_path << endl;
    cout << "Archivo de salida: " << output_file << endl;
    cout << "==========================================" << endl << endl;

    unordered_map<string, vector<DocumentCount>> index;
    index.reserve(100000);

    // Contar archivos a procesar
    int total_archivos = 0;
    int archivos_procesados = 0;
    DIR *dir_count = opendir(folder_path.c_str());
    if (dir_count != nullptr) {
        struct dirent *entry;
        while ((entry = readdir(dir_count)) != nullptr) {
            string filename = entry->d_name;
            if (filename != "." && filename != ".." && filename[0] != '.') {
                string filepath = folder_path;
                filepath += "/";
                filepath += filename;

                struct stat path_stat = {};
                stat(filepath.c_str(), &path_stat);
                if (S_ISREG(path_stat.st_mode)) {
                    total_archivos++;
                }
            }
        }
        closedir(dir_count);
    }

    cout << "Encontrados " << total_archivos << " archivos para procesar" << endl << endl;

    // Procesar archivos
    DIR *dir = opendir(folder_path.c_str());
    if (dir != nullptr) {
        struct dirent *entry;
        while ((entry = readdir(dir)) != nullptr) {
            string filename = entry->d_name;

            if (filename == "." || filename == ".." || filename[0] == '.') {
                continue;
            }

            string filepath = folder_path;
            filepath += "/";
            filepath += filename;

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
    }

    cout << "\n✓ Procesamiento de archivos completado" << endl;
    cout << "Escribiendo índice en archivo..." << endl;

    ofstream outfile(output_file);
    if (!outfile.is_open()) {
        cout << "✗ Error: No se pudo crear el archivo: " << output_file << endl;
        return 1;
    }

    // Ordenar palabras alfabéticamente para mejor legibilidad
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
    cout << "✓ Índice creado: " << output_file << endl;
    cout << "✓ Total de palabras únicas: " << index.size() << endl;
    cout << "✓ Archivos procesados: " << archivos_procesados << "/" << total_archivos << endl;
    cout << "==========================================" << endl;

    cout << "\nPresione Enter para continuar...";
    cin.ignore();
    cin.get();

    return 0;
}