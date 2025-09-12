#include "../include/AppConfig.h"

using namespace std;


// Variable global que almacena la ruta al archivo de usuarios
string g_usuariosFile = "";


// Establece la ruta del archivo de usuarios globalmente
void setUsuariosFile(const string& file) {
    g_usuariosFile = file;
}


// Devuelve la ruta actual del archivo de usuarios
const string& getUsuariosFile() {
    return g_usuariosFile;
}