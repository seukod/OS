#ifndef INPUT_UTILS_H
#define INPUT_UTILS_H

#include <string>

// Función universal para leer opciones de menús
int leerOpcion();

// Lee un entero mostrando un mensaje personalizado. Valida la entrada.
int obtenerEntero(const std::string& mensaje);

#endif // INPUT_UTILS_H
