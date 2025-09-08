#ifndef MENU_PRINCIPAL_H
#define MENU_PRINCIPAL_H

#include "../users_auth.h"
#include <string>

using namespace std;

// Funciones del menú principal
void mostrarMenuPrincipal(const Usuario& usuario);
void ejecutarMenuPrincipal(const Usuario& usuario, const string& libro);

// Funciones para las opciones del menú
void mostrarEnConstruccion(const string& funcionalidad);

#endif // MENU_PRINCIPAL_H
