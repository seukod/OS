#ifndef TEXTO_H
#define TEXTO_H

#include <string>
#include <vector>
using namespace std;

// Definición del struct Usuario
struct Usuario {
    int id;
    string nombre;
    string username;
    string password;
    string perfil;
};

string leerVariableEnv(const string& nombreVariable, const string& archivoEnv = ".env");

#endif // TEXTO_H
