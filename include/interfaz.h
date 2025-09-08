//
// Created by Francisco Contreras on 27-08-25.
//

#ifndef OS_UI_FUNC_H
#define OS_UI_FUNC_H

#include <string>

using namespace std;

// Funciones de interfaz principal
void mostrarBienvenida();
void mostrarMensajeDespedida();

// Funciones de mensajes
void mostrarMensajeError(const string& mensaje);
void mostrarMensajeExito(const string& mensaje);
void mostrarMensajeAdvertencia(const string& mensaje);

// Funciones utilidades de pantalla
void pausarPantalla();
void limpiarPantalla();
void limpiarBuffer();
void mostrarSeparador();
void mostrarTitulo(const string& titulo);

#endif //OS_UI_FUNC_H
