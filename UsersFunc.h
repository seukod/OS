#ifndef OS_USERSFUNC_H
#define OS_USERSFUNC_H
#include <string>
#include "centro.h"
// funciones por implementar
string leerVariableEnv(const string& nombreVariable, const string& archivoEnv = ".env");

void ingresarUsuario(vector<Usuario>& usuarios, const string& filePath);
void eliminarUsuario(vector<Usuario>& usuarios, const string& filePath);
void listarUsuarios(vector<Usuario>& usuarios);
void ingresarPerfil();
void eliminarPerfil();
void listarPerfiles();


#endif
