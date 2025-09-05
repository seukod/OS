#include "../include/ArgParser.h"
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
            opts.book = argv[++i];
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
    cout << "Uso: " << programName << " -u <usuario> -p <password> -f <libro.txt>\n\n";
    cout << "Opciones obligatorias:\n";
    cout << "  -u <usuario>    Nombre de usuario\n";
    cout << "  -p <password>   Contraseña del usuario\n";
    cout << "  -f <libro.txt>  Nombre del libro a leer (con extensión .txt)\n\n";
    cout << "Opciones adicionales:\n";
    cout << "  -h, --help      Mostrar esta ayuda\n\n";
}

void validateOptions(const Options& opts) {
    if (opts.user.empty()) {
        throw runtime_error("Error: Usuario (-u) es obligatorio");
    }
    
    if (opts.password.empty()) {
        throw runtime_error("Error: Password (-p) es obligatorio");
    }
    
    if (opts.book.empty()) {
        throw runtime_error("Error: Nombre del libro (-f) es obligatorio");
    }
    
    if (!isValidBookName(opts.book)) {
        throw runtime_error("Error: El archivo debe tener extensión .txt");
    }
    
    // Verificar que el archivo del libro existe en data/libros/
    string bookPath = "../data/libros/" + opts.book;
    ifstream file(bookPath);
    if (!file.good()) {
        throw runtime_error("Error: No se puede acceder al libro: " + opts.book);
    }
    file.close();
}

bool isValidBookName(const string& bookname) {
    if (bookname.empty()) {
        return false;
    }
    
    // Verificar que tenga extensión .txt
    if (bookname.length() < 4) {
        return false;
    }
    if (bookname.substr(bookname.length() - 4) != ".txt") {
        return false;
    }
    
    // Verificar que no contenga caracteres inválidos para nombres de archivo
    string invalidChars = "<>:\"|?*";
    for (char c : invalidChars) {
        if (bookname.find(c) != string::npos) {
            return false;
        }
    }
    return true;
}