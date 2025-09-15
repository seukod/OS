#include "../include/interfaz.h"
#include "../../include/menus/menu_funcion.h"
#include <iostream>
#include <unistd.h>

using namespace std;

void calcular_f() {
    cout << "PID DEL PROCESO: " << getpid() << endl;
    while (true) {
        mostrarTitulo("Calculadora de f(x) = x*x + 2x + 8");

        cout << "Ingrese un número para calcular f(x): ";

        float x;
        while (!(cin >> x)) {
            mostrarMensajeError("Entrada inválida. Por favor ingrese un número válido.");
            limpiarBuffer();
            cout << "Ingrese un número para calcular f(x): ";
        }
        limpiarBuffer();

        float resultado = x*x + 2*x + 8;
        cout << "El resultado de f(" << x << ") = " << resultado << endl;

        cout << endl << "¿Desea calcular otro número? (1 = Sí, 0 = No): ";
        int opcion;
        while (!(cin >> opcion) || (opcion != 0 && opcion != 1)) {
            mostrarMensajeError("Opción inválida. Ingrese 1 para sí o 0 para no.");
            limpiarBuffer();
            cout << "¿Desea calcular otro número? (1 = Sí, 0 = No): ";
        }
        limpiarBuffer();

        if (opcion == 0) break;
    }
    return;
}
