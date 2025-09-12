// Implementación de utilidades para manejo de entrada de datos del usuario
#include "../include/utils/input_utils.h"
#include "../include/interfaz.h"
#include <iostream>

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
