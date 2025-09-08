#include "../include/utils/input_utils.h"
#include "../include/interfaz.h"
#include <iostream>

using namespace std;

int leerOpcion() {
    int opcion;
    while(!(cin >> opcion)) {
        mostrarMensajeError("Entrada inválida. Por favor ingrese un número.");
        limpiarBuffer();
        cout << "Seleccione una opción: ";
    }
    limpiarBuffer();
    return opcion;
}
