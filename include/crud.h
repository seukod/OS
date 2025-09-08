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

// Definición del struct Perfil
struct Perfil {
    string nombre;
    vector<int> permisos;
};

// Variables globales para almacenar datos en memoria
extern vector<Usuario> g_usuarios;
extern vector<Perfil> g_perfiles;

// Funciones para manejo de memoria y carga de datos
void limpiarMemoria();
bool cargarDatosEnMemoria();
bool guardarCambios();
string leerVariableEnv(const string& nombreVariable, const string& archivoEnv = ".env");

// Funciones de validación
bool validarUsuario(const string& username, const string& password, Usuario& user);

#endif // CRUD_H
