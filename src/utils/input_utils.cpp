// Implementación de utilidades para manejo de entrada de datos del usuario
#include "../include/utils/input_utils.h"
#include "../include/interfaz.h"
#include <iostream>
#include <string>

using namespace std;

// Lee una opción numérica del usuario validando que sea un número entero válido.
// Si la entrada es inválida, muestra un mensaje de error y solicita nuevamente la entrada.
int leerOpcion() {
    int opcion;
    while(!(cin >> opcion)) {
        mostrarMensajeError("Entrada inválida. Por favor ingrese un número.");
        limpiarBuffer(); // Limpia el buffer de entrada para evitar bucles infinitos
        cout << "Seleccione una opción: ";
    }
    limpiarBuffer(); // Limpia cualquier caracter restante en el buffer
    return opcion;
}

// Lee un entero mostrando un mensaje personalizado y validando la entrada.
int obtenerEntero(const std::string& mensaje) {
    int valor;
    while (true) {
        cout << mensaje;
        if (cin >> valor) {
            limpiarBuffer();
            return valor;
        } else {
            mostrarMensajeError("Entrada inválida. Por favor ingrese un número entero.");
            limpiarBuffer();
        }
    }
}
