#ifndef MENU_H
#define MENU_H

#include <string>
#include <vector>
using namespace std;

struct Usuario {
    int id;
    string nombre;
    string username;
    string password;
    string perfil;
};

// Prototipos de funciones
string getUserFilePath(const string& envFile);
vector<Usuario> leerUsuarios(const string& filePath);
void guardarCambios(const vector<Usuario>& usuarios, const string& filePath);

#endif
