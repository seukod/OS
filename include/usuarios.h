#ifndef USUARIOS_H
#define USUARIOS_H

#include "crud.h"
#include <vector>

// Funciones de usuarios
void ingresarUsuarioEnMemoria();
void eliminarUsuarioEnMemoria();
void listarUsuarios(const vector<Usuario>& usuarios);

// Funciones de perfiles
void ingresarPerfilEnMemoria();
void eliminarPerfilEnMemoria();
void listarPerfilesEnMemoria();

#endif // USUARIOS_H
