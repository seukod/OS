#ifndef ARGPARSER_H
#define ARGPARSER_H

#include <string>

struct Options {
    std::string user;
    std::string password;
    std::string book;  // Cambiado de 'file' a 'book' para el nombre del libro
    bool help = false;
};

// Funciones para parsear argumentos
Options parseArguments(int argc, char* argv[]);
void showHelp(const std::string& programName);
void validateOptions(const Options& opts);
bool isValidBookName(const std::string& bookname);

#endif // ARGPARSER_H