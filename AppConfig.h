#pragma once
#include <string>

namespace AppConfig {
    void setUsuariosFile(const std::string& path);
    const std::string& getUsuariosFile();
}
