//
// Created by Francisco Contreras on 27-08-25.
//

#include "../include/interfaz.h"
#include "../include/utils/input_utils.h"
#include <iostream>
#include <limits>

using namespace std;

void mostrarBienvenida() {
    limpiarPantalla();
    cout << "=================================================" << endl;
    cout << "         	      MENÚ PRINCIPAL                  " << endl;
    cout << "=================================================" << endl;
    cout << endl;
}

void mostrarMensajeError(const string& mensaje) {
    cout << endl;
    cout << "ERROR: " << mensaje << endl;
    cout << endl;
}

void mostrarMensajeExito(const string& mensaje) {
    cout << endl;
    cout << "ÉXITO: " << mensaje << endl;
    cout << endl;
}

void mostrarMensajeAdvertencia(const string& mensaje) {
    cout << endl;
    cout << "ADVERTENCIA: " << mensaje << endl;
    cout << endl;
}

void mostrarMensajeDespedida() {
    limpiarPantalla();
    cout << "=================================================" << endl;
    cout << "			HASTA LUEGO	                         " << endl;
    cout << "=================================================" << endl;
}

void pausarPantalla() {
    cout << endl;
    cout << "Presione Enter para continuar...";
    cin.get();
}

void limpiarPantalla() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void limpiarBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void mostrarSeparador() {
    cout << "-------------------------------------------------" << endl;
}

void mostrarTitulo(const string& titulo) {
    cout << endl;
    cout << "=================================================" << endl;
    cout << "              " << titulo << "                  " << endl;
    cout << "=================================================" << endl;
    cout << endl;
}
