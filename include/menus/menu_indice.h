#ifndef MENU_INDICE_H
#define MENU_INDICE_H

#include <string>
using namespace std;

// Función principal que muestra la interfaz y ejecuta el programa de creación de índice
void ejecutarMenuIndice();

// Función que valida el nombre del archivo (debe terminar en .idx)
bool validarNombreArchivo(const string& nombreArchivo);

// Función que valida que el directorio existe
bool validarDirectorio(const string& pathDirectorio);

// Función que llama al programa externo para crear el índice invertido
bool crearIndiceInvertido(const string& nombreArchivo, const string& pathCarpeta);

#endif // MENU_INDICE_H
