#ifndef MENU_MATRIZ_H
#define MENU_MATRIZ_H

#include <string>
using namespace std;

// Función principal que muestra la interfaz de configuración de matrices
void ejecutarMenuMatriz();

// Función que valida la existencia de un archivo de matriz
bool validarMatriz(const string& archivo);

// Función que convierte una ruta relativa a absoluta
string convertirRutaAbsoluta(const string& ruta);

#endif // MENU_MATRIZ_H
