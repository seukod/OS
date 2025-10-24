#ifndef OS_MENU_INDICE_INV_H
#define OS_MENU_INDICE_INV_H

#include <string>

using namespace std;

// Función principal del menú de índice invertido paralelo
void ejecutarMenuIndiceInv();

// Funciones auxiliares
bool validarNombreArchivoIdx(const string& nombreArchivo);
bool validarDirectorioLibros(const string& pathDirectorio);
bool crearIndiceInvertidoParalelo(const string& nombreArchivo, const string& pathCarpeta);

#endif //OS_MENU_INDICE_INV_H
