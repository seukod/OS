#ifndef USUARIOS_H
#define USUARIOS_H

#include "crud.h"
#include <vector>

// Funciones de usuarios
void ingresarUsuario();
void eliminarUsuario();
void listarUsuarios(const vector<Usuario>& usuarios);

// Funciones de perfiles
void ingresarPerfil();
void eliminarPerfil();
void listarPerfiles();

#endif // USUARIOS_H
