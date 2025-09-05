#include "crud.h"
#include "AppConfig.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cctype>
#include <vector>
#include <sstream>
using namespace std;

static inline void ltrim(string &s) { while (!s.empty() && isspace((unsigned char) s.front())) s.erase(s.begin()); }
static inline void rtrim(string &s) { while (!s.empty() && isspace((unsigned char) s.back())) s.pop_back(); }

static inline void trim(string &s) {
    ltrim(s);
    rtrim(s);
}

static inline string stripQuotes(string v) {
    if (v.size() >= 2 && ((v.front() == '"' && v.back() == '"') || (v.front() == '\'' && v.back() == '\''))) {
        return v.substr(1, v.size() - 2);
    }
    return v;
}

// Lee una variable del entorno o desde .env (intenta ./.env y ../.env)
string leerVariableEnv(const string &nombreVariable, const string &archivoEnv ) {
    if (const char *envv = std::getenv(nombreVariable.c_str())) {
        return string(envv);
    }

    auto intenta = [&](const string &path) -> string {
        ifstream envFile(path);
        if (!envFile.is_open()) return "";
        string linea;
        while (getline(envFile, linea)) {
            trim(linea);
            if (linea.empty() || linea[0] == '#') continue;
            size_t pos = linea.find('=');
            if (pos == string::npos) continue;
            string key = linea.substr(0, pos);
            string value = linea.substr(pos + 1);
            trim(key);
            trim(value);
            value = stripQuotes(value);
            if (key == nombreVariable) return value;
        }
        return "";
    };

    string v = intenta(archivoEnv);
    if (!v.empty()) return v;
    return intenta("../.env");
}
vector<Usuario> crear_arreglo() {
    // Primero intentar usar el archivo configurado desde AppConfig
    string userFile = getUsuariosFile();
    
    // Si no hay archivo configurado, intentar leer desde .env como fallback
    if (userFile.empty()) {
        userFile = leerVariableEnv("USERS_FILE", ".env");
    }
    
    vector<Usuario> usuarios;

    if (userFile.empty()) {
        cerr << "[ERROR] No se encontró archivo de usuarios configurado." << endl;
        return usuarios;
    }

    ifstream file(userFile);
    if (!file.is_open()) {
        cerr << "[ERROR] No se pudo abrir archivo de usuarios: " << userFile << endl;
        return usuarios;
    }

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        Usuario u;
        string idStr;

        if (getline(ss, idStr, ',') &&
            getline(ss, u.nombre, ',') &&
            getline(ss, u.username, ',') &&
            getline(ss, u.password, ',') &&
            getline(ss, u.perfil, ',')) {

            try {
                u.id = stoi(idStr);
                usuarios.push_back(u);
            } catch (...) {
                cerr << "[ADVERTENCIA] ID inválido en línea: " << line << endl;
            }
        }
    }

    return usuarios;
}

bool validarUsuario(const string& username, const string& password, Usuario& user) {
    // Usar el archivo configurado desde AppConfig
    string userFile = getUsuariosFile();
    
    // Si no hay archivo configurado, intentar leer desde .env como fallback
    if (userFile.empty()) {
        userFile = leerVariableEnv("USERS_FILE", ".env");
    }
    
    if (userFile.empty()) {
        cerr << "[ERROR] No se encontró archivo de usuarios configurado." << endl;
        return false;
    }

    ifstream file(userFile);
    if (!file.is_open()) {
        cerr << "[ERROR] No se pudo abrir archivo de usuarios: " << userFile << endl;
        return false;
    }

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        Usuario u;
        string idStr;

        if (getline(ss, idStr, ',') &&
            getline(ss, u.nombre, ',') &&
            getline(ss, u.username, ',') &&
            getline(ss, u.password, ',') &&
            getline(ss, u.perfil, ',')) {

            try {
                u.id = stoi(idStr);
                
                // Verificar credenciales
                if (u.username == username && u.password == password) {
                    user = u;  // Copiar datos del usuario encontrado
                    file.close();
                    return true;
                }
            } catch (...) {
                cerr << "[ADVERTENCIA] ID inválido en línea: " << line << endl;
            }
        }
    }

    file.close();
    return false;  // Usuario no encontrado o credenciales incorrectas
}
