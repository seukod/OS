#include "AppConfig.h"

namespace {
    std::string g_usuariosFile = "USUARIOS.txt"; // valor por defecto
}

namespace AppConfig {
    void setUsuariosFile(const std::string& path) { g_usuariosFile = path; }
    const std::string& getUsuariosFile() { return g_usuariosFile; }
}
