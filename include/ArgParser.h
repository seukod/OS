#ifndef ARGPARSER_H
#define ARGPARSER_H

#include <string>

struct Options {
    std::string user;
    std::string password;
    std::string file;
    bool help = false;
};

// Funciones para parsear argumentos
Options parseArguments(int argc, char* argv[]);
void showHelp(const std::string& programName);
void validateOptions(const Options& opts);
bool isValidTxtFile(const std::string& filename);

#endif // ARGPARSER_H