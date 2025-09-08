#include "../include/users_auth.h"
#include "../include/AppConfig.h"
#include "../include/utils/string_utils.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cctype>
#include <vector>
#include <sstream>
using namespace std;

// Variables globales para almacenar datos en memoria
vector<Usuario> g_usuarios;
vector<Perfil> g_perfiles;

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

bool validarUsuario(const string& username, const string& password, Usuario& user) {
    // Buscar usuario en los datos cargados en memoria
    for (const auto& u : g_usuarios) {
        if (u.username == username && u.password == password) {
            user = u;  // Copiar datos del usuario encontrado
            return true;
        }
    }

    return false;  // Usuario no encontrado o credenciales incorrectas
}

// Función para limpiar la memoria
void limpiarMemoria() {
    g_usuarios.clear();
    g_perfiles.clear();
    cout << "[INFO] Memoria limpiada correctamente." << endl;
}

// Función para cargar datos de USUARIOS.txt y PERFILES.txt en memoria
bool cargarDatosEnMemoria() {
    // Limpiar memoria primero
    limpiarMemoria();
    
    // Cargar usuarios desde data/USUARIOS.txt
    string rutaUsuarios = leerVariableEnv("USERS_FILE");
    if (rutaUsuarios.empty()) {
        rutaUsuarios = "../data/USUARIOS.txt";  // Ruta relativa desde build/
    }
    
    ifstream fileUsuarios(rutaUsuarios);
    if (!fileUsuarios.is_open()) {
        cerr << "[ERROR] No se pudo abrir archivo de usuarios." << endl;
        return false;
    }
    
    string line;
    while (getline(fileUsuarios, line)) {
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
                g_usuarios.push_back(u);
            } catch (...) {
                cerr << "[ADVERTENCIA] ID inválido en línea: " << line << endl;
            }
        }
    }
    fileUsuarios.close();
    
    // Cargar perfiles desde data/PERFILES.txt
    string rutaPerfiles = leerVariableEnv("PROFILES_FILE");
    if (rutaPerfiles.empty()) {
        rutaPerfiles = "../data/PERFILES.txt";  // Ruta relativa desde build/
    }
    
    ifstream filePerfiles(rutaPerfiles);
    if (!filePerfiles.is_open()) {
        cerr << "[ERROR] No se pudo abrir archivo de perfiles." << endl;
        return false;
    }
    
    while (getline(filePerfiles, line)) {
        if (line.empty()) continue;

        size_t pos = line.find(';');
        if (pos == string::npos) continue;

        Perfil p;
        p.nombre = line.substr(0, pos);
        string permisosStr = line.substr(pos + 1);

        // Parsear permisos separados por coma
        stringstream ss(permisosStr);
        string permiso;
        while (getline(ss, permiso, ',')) {
            try {
                p.permisos.push_back(stoi(permiso));
            } catch (...) {
                cerr << "[ADVERTENCIA] Permiso inválido: " << permiso << endl;
            }
        }

        g_perfiles.push_back(p);
    }
    filePerfiles.close();
    
    cout << "[INFO] Datos cargados en memoria correctamente." << endl;

    return true;
}

// Función para guardar todos los cambios de memoria a los archivos
bool guardarCambios() {
    cout << "[INFO] Guardando cambios en archivos..." << endl;

    // Guardar usuarios
    string rutaUsuarios = leerVariableEnv("USERS_FILE");
    if (rutaUsuarios.empty()) {
        rutaUsuarios = "../data/USUARIOS.txt";  // Ruta relativa desde build/
    }
    
    ofstream fileUsuarios(rutaUsuarios, ios::trunc);
    if (!fileUsuarios.is_open()) {
        cerr << "[ERROR] No se pudo abrir archivo de usuarios para escritura." << endl;
        return false;
    }
    
    for (const auto& usuario : g_usuarios) {
        fileUsuarios << usuario.id << "," << usuario.nombre << ","
                    << usuario.username << "," << usuario.password << ","
                    << usuario.perfil << endl;
    }
    fileUsuarios.close();
    
    // Guardar perfiles
    string rutaPerfiles = leerVariableEnv("PROFILES_FILE");
    if (rutaPerfiles.empty()) {
        rutaPerfiles = "../data/PERFILES.txt";  // Ruta relativa desde build/
    }
    
    ofstream filePerfiles(rutaPerfiles, ios::trunc);
    if (!filePerfiles.is_open()) {
        cerr << "[ERROR] No se pudo abrir archivo de perfiles para escritura." << endl;
        return false;
    }
    
    for (const auto& perfil : g_perfiles) {
        filePerfiles << perfil.nombre << ";";
        for (size_t i = 0; i < perfil.permisos.size(); i++) {
            filePerfiles << perfil.permisos[i];
            if (i < perfil.permisos.size() - 1) {
                filePerfiles << ",";
            }
        }
        filePerfiles << endl;
    }
    filePerfiles.close();
    
    cout << "[INFO] Cambios guardados exitosamente." << endl;

    return true;
}
