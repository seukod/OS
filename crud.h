#ifndef CRUD_H
#define CRUD_H

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
vector<Usuario> crear_arreglo();
bool validarUsuario(const string& username, const string& password, Usuario& user);

#endif // CRUD_H
