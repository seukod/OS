#ifndef ADMIN_USUARIOS_H
#define ADMIN_USUARIOS_H


#include "../../include/users_auth.h"
#include "../../include/usuarios.h"
#include "../../include/interfaz.h"
#include "../../include/utils/input_utils.h"
#include <iostream>

using namespace std;

// Funciones principales de la aplicación
void mostrarMenuPrincipal();
void ejecutarAplicacion();

// Funciones adicionales específicas para esta aplicación standalone
void mostrarTituloAplicacion();
void procesarOpcionMenu(int opcion);

#endif // ADMIN_USUARIOS_H
