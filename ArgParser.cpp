#include "ArgParser.h"
#include <iostream>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

static bool hasTxtExtension(const std::string& path) {
    if (path.size() < 4) return false;
    std::string ext = path.substr(path.size() - 4);
    std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c){ return std::tolower(c); });
    return ext == ".txt";
}

void ArgParser::printUsage(const char* programName) {
    std::cerr
        << "Uso: " << programName << " -u <usuario> -p <password> -f <archivo.txt>\n"
        << "Opciones:\n"
        << "  -u, --user     Usuario de inicio de sesion\n"
        << "  -p, --pass     Password de inicio de sesion\n"
        << "  -f, --file     Ruta al archivo .txt de usuarios\n"
        << "  -h, --help     Mostrar esta ayuda\n";
}

Options ArgParser::parse(int argc, char* argv[]) {
    Options opts;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        auto requireValue = [&](const char* flag) {
            if (i + 1 >= argc) {
                std::cerr << "Falta valor para " << flag << "\n";
                printUsage(argv[0]);
                std::exit(1);
            }
        };

        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            std::exit(0);
        } else if (arg == "-u" || arg == "--user") {
            requireValue(arg.c_str());
            opts.user = argv[++i];
        } else if (arg == "-p" || arg == "--pass") {
            requireValue(arg.c_str());
            opts.password = argv[++i];
        } else if (arg == "-f" || arg == "--file") {
            requireValue(arg.c_str());
            opts.file = argv[++i];
        } else {
            std::cerr << "Argumento desconocido: " << arg << "\n";
            printUsage(argv[0]);
            std::exit(1);
        }
    }

    if (opts.user.empty() || opts.password.empty() || opts.file.empty()) {
        std::cerr << "Error: Debes proporcionar -u, -p y -f.\n";
        printUsage(argv[0]);
        std::exit(1);
    }

    std::error_code ec;
    if (!fs::exists(opts.file, ec) || fs::is_directory(opts.file, ec)) {
        std::cerr << "Error: el archivo no existe o es un directorio: " << opts.file << "\n";
        std::exit(1);
    }
    if (!hasTxtExtension(opts.file)) {
        std::cerr << "Error: el archivo debe tener extension .txt: " << opts.file << "\n";
        std::exit(1);
    }

    return opts;
}
