// Implementación de utilidades para manejo de entrada de datos del usuario
#include "../include/utils/input_utils.h"
#include "../include/interfaz.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>

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
std::string limpiarCadena(const std::string& str) {
    // 1. Crear una copia modificable de la cadena de entrada.
    std::string cadena_limpia = str;
    // 2. Aplicar la lógica de limpieza a la copia (lo que permite modificarla).
    cadena_limpia.erase(
        std::find_if(cadena_limpia.rbegin(), cadena_limpia.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(),
        cadena_limpia.end()
    );
    // 3. Retornar la cadena limpia.
    return cadena_limpia;
}

std::string windowsToLinuxPath(const std::string& winPath) {
    std::string lin = winPath;

    // 1. Convertir \ a /
    std::replace(lin.begin(), lin.end(), '\\', '/');

    // 2. Detectar unidad Ej: "C:"
    if (lin.size() > 2 && lin[1] == ':') {
        char drive = std::tolower(lin[0]); // c → minúscula

        // Convertir a /mnt/<unidad>/ruta
        lin = "/mnt/" + std::string(1, drive) + lin.substr(2);
    }

    return lin;
}