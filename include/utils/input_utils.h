#ifndef INPUT_UTILS_H
#define INPUT_UTILS_H
#include <string>
// Función universal para leer opciones de menús
int leerOpcion();
std::string limpiarCadena(const std::string& str);
std::string windowsToLinuxPath(const std::string& winPath);
#endif // INPUT_UTILS_H
