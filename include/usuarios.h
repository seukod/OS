#ifndef USUARIOS_H
#define USUARIOS_H

#include "users_auth.h"
#include <vector>

// Funciones de usuarios
void ingresarUsuarioEnMemoria();
void eliminarUsuarioEnMemoria();
void listarUsuarios();

// Funciones de perfiles
void ingresarPerfilEnMemoria();
void eliminarPerfilEnMemoria();
void listarPerfilesEnMemoria();

// Funciones auxiliares de validación
int contarUsuariosConPerfil(const string& perfil);
bool esPerfilProtegido(const string& nombrePerfil);

#endif // USUARIOS_H
