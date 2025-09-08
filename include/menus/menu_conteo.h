#ifndef MENU_CONTEO_H
#define MENU_CONTEO_H

#include <string>
using namespace std;
void mostrarInterfazConteo(const string& filePath);
void contarArchivo(const string& filePath, int& vocales, int& consonantes, int& especiales, int& palabras);

#endif
