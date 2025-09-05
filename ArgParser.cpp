#include "ArgParser.h"
#include <iostream>
#include <cstring>
#include <stdexcept>
#include <fstream>

using namespace std;

Options parseArguments(int argc, char* argv[]) {
    Options opts;
    
    if (argc == 1) {
        showHelp(argv[0]);
        exit(1);
    }
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-u") == 0) {
            if (i + 1 >= argc) {
                throw runtime_error("Error: -u requiere un valor");
            }
            opts.user = argv[++i];
        }
        else if (strcmp(argv[i], "-p") == 0) {
            if (i + 1 >= argc) {
                throw runtime_error("Error: -p requiere un valor");
            }
            opts.password = argv[++i];
        }
        else if (strcmp(argv[i], "-f") == 0) {
            if (i + 1 >= argc) {
                throw runtime_error("Error: -f requiere un valor");
            }
            opts.file = argv[++i];
        }
        else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            opts.help = true;
            showHelp(argv[0]);
            exit(0);
        }
        else {
            throw runtime_error("Error: Opción desconocida: " + string(argv[i]));
        }
    }
    
    validateOptions(opts);
    return opts;
}

void showHelp(const string& programName) {
    cout << "Uso: " << programName << " -u <usuario> -p <password> -f <archivo>\n\n";
    cout << "Opciones obligatorias:\n";
    cout << "  -u <usuario>    Nombre de usuario\n";
    cout << "  -p <password>   Contraseña del usuario\n";
    cout << "  -f <archivo>    Archivo de texto (.txt) con datos de usuarios\n\n";
    cout << "Opciones adicionales:\n";
    cout << "  -h, --help      Mostrar esta ayuda\n\n";
    cout << "Ejemplo:\n";
    cout << "  " << programName << " -u admin -p 123456 -f usuarios.txt\n";
}

void validateOptions(const Options& opts) {
    if (opts.user.empty()) {
        throw runtime_error("Error: Usuario (-u) es obligatorio");
    }
    
    if (opts.password.empty()) {
        throw runtime_error("Error: Password (-p) es obligatorio");
    }
    
    if (opts.file.empty()) {
        throw runtime_error("Error: Archivo (-f) es obligatorio");
    }
    
    if (!isValidTxtFile(opts.file)) {
        throw runtime_error("Error: El archivo debe tener extensión .txt");
    }
    
    // Verificar que el archivo existe
    ifstream file(opts.file);
    if (!file.good()) {
        throw runtime_error("Error: No se puede acceder al archivo: " + opts.file);
    }
    file.close();
}

bool isValidTxtFile(const string& filename) {
    if (filename.length() < 4) {
        return false;
    }
    return filename.substr(filename.length() - 4) == ".txt";
}