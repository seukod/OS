#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <string>

// Variable global para el archivo de usuarios
extern std::string g_usuariosFile;

// Funciones para manejar la configuración
void setUsuariosFile(const std::string& file);
const std::string& getUsuariosFile();

#endif // APPCONFIG_H