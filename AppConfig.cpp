#include "AppConfig.h"

using namespace std;

// Variable global
string g_usuariosFile = "";

void setUsuariosFile(const string& file) {
    g_usuariosFile = file;
}

const string& getUsuariosFile() {
    return g_usuariosFile;
}