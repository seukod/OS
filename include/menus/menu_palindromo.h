#ifndef MENU_PALINDROMO_H
#define MENU_PALINDROMO_H

#include <string>

using namespace std;

// Funciones del menú de palíndromo
void mostrarMenuPalindromo();
void ejecutarMenuPalindromo();
bool esPalindromo(const string& texto);
string limpiarTexto(const string& texto);

#endif // MENU_PALINDROMO_H