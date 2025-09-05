#pragma once
#include <string>

struct Options {
    std::string user;
    std::string password;
    std::string file;
};

class ArgParser {
public:
    // Parsea y valida argumentos. Sale con código != 0 en caso de error.
    static Options parse(int argc, char* argv[]);
    static void printUsage(const char* programName);
};
