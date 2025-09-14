//
// Created by Francisco Contreras on 13-09-25.
//

#ifndef OS_MENU_MATRIZ_H
#define OS_MENU_MATRIZ_H
#include "../include/interfaz.h"
#include "../include/utils/input_utils.h"
#include "users_auth.h"
#include <iostream>
#include <string>
using namespace std;
bool validarExistenciaArchivo(const string& archivo);
void ejecutarMultiplicador(const string& multiplicadorPath, const string& archivo1, const string& archivo2);
void menuMatriz();

#endif //OS_MENU_MATRIZ_H