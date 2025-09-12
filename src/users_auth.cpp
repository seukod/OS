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


// Variables globales para almacenar en memoria los usuarios y perfiles cargados desde los archivos.
// Esto permite acceder y modificar los datos sin leer/escribir constantemente los archivos.
vector<Usuario> g_usuarios;
vector<Perfil> g_perfiles;


// Lee una variable de entorno. Si no existe, intenta buscarla en un archivo .env (primero el dado, luego ../.env).
// Devuelve el valor de la variable si se encuentra, o una cadena vacía si no existe.
string leerVariableEnv(const string &nombreVariable, const string &archivoEnv ) {
    // Primero intenta obtener la variable del entorno del sistema
    if (const char *envv = std::getenv(nombreVariable.c_str())) {
        return string(envv);
    }

    // Si no está en el entorno, busca en el archivo .env
    auto intenta = [&](const string &path) -> string {
        ifstream envFile(path);
        if (!envFile.is_open()) return "";
        string linea;
        while (getline(envFile, linea)) {
            trim(linea);
            if (linea.empty() || linea[0] == '#') continue; // Ignora comentarios y líneas vacías
            size_t pos = linea.find('=');
            if (pos == string::npos) continue;
            string key = linea.substr(0, pos);
            string value = linea.substr(pos + 1);
            trim(key);
            trim(value);
            value = stripQuotes(value); // Quita comillas si las hay
            if (key == nombreVariable) return value;
        }
        return "";
    };

    // Intenta primero con el archivo dado, luego con ../.env
    string v = intenta(archivoEnv);
    if (!v.empty()) return v;
    return intenta("../.env");
}


// Valida si un usuario y contraseña existen en la lista de usuarios cargados en memoria.
// Si es válido, copia los datos del usuario encontrado en 'user' y retorna true.
// Si no, retorna false.
bool validarUsuario(const string& username, const string& password, Usuario& user) {
    for (const auto& u : g_usuarios) {
        if (u.username == username && u.password == password) {
            user = u;  // Copiar datos del usuario encontrado
            return true;
        }
    }
    return false;  // Usuario no encontrado o credenciales incorrectas
}


// Limpia los vectores globales de usuarios y perfiles, liberando la memoria ocupada.
void limpiarMemoria() {
    g_usuarios.clear();
    g_perfiles.clear();
    cout << "[INFO] Memoria limpiada correctamente." << endl;
}


// Carga los datos de usuarios y perfiles desde los archivos de texto a memoria.
// Lee USUARIOS.txt y PERFILES.txt (o los archivos definidos por variables de entorno).
// Devuelve true si la carga fue exitosa, false si hubo algún error.
bool cargarDatosEnMemoria() {
    limpiarMemoria(); // Limpiar memoria primero
    
    // Cargar usuarios desde archivo
    string rutaUsuarios = leerVariableEnv("USERS_FILE");
    if (rutaUsuarios.empty()) {
        rutaUsuarios = "../data/USUARIOS.txt";  // Ruta por defecto
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
        
        // Espera el formato: id,nombre,username,password,perfil
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
    
    // Cargar perfiles desde archivo
    string rutaPerfiles = leerVariableEnv("PROFILES_FILE");
    if (rutaPerfiles.empty()) {
        rutaPerfiles = "../data/PERFILES.txt";  // Ruta por defecto
    }
    
    ifstream filePerfiles(rutaPerfiles);
    if (!filePerfiles.is_open()) {
        cerr << "[ERROR] No se pudo abrir archivo de perfiles." << endl;
        return false;
    }
    
    while (getline(filePerfiles, line)) {
        if (line.empty()) continue;
        
        // Espera el formato: nombre;permiso1,permiso2,...
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


// Guarda los datos de usuarios y perfiles desde memoria a los archivos de texto.
// Sobrescribe los archivos USUARIOS.txt y PERFILES.txt (o los definidos por variables de entorno).
// Devuelve true si la operación fue exitosa, false si hubo algún error.
bool guardarCambios() {
    cout << "[INFO] Guardando cambios en archivos..." << endl;
    
    // Guardar usuarios
    string rutaUsuarios = leerVariableEnv("USERS_FILE");
    if (rutaUsuarios.empty()) {
        rutaUsuarios = "../data/USUARIOS.txt";  // Ruta por defecto
    }
    
    ofstream fileUsuarios(rutaUsuarios, ios::trunc);
    if (!fileUsuarios.is_open()) {
        cerr << "[ERROR] No se pudo abrir archivo de usuarios para escritura." << endl;
        return false;
    }
    
    // Escribir cada usuario en una línea
    for (const auto& usuario : g_usuarios) {
        fileUsuarios << usuario.id << "," << usuario.nombre << "," 
                    << usuario.username << "," << usuario.password << "," 
                    << usuario.perfil << endl;
    }
    fileUsuarios.close();
    
    // Guardar perfiles
    string rutaPerfiles = leerVariableEnv("PROFILES_FILE");
    if (rutaPerfiles.empty()) {
        rutaPerfiles = "../data/PERFILES.txt";  // Ruta por defecto
    }
    
    ofstream filePerfiles(rutaPerfiles, ios::trunc);
    if (!filePerfiles.is_open()) {
        cerr << "[ERROR] No se pudo abrir archivo de perfiles para escritura." << endl;
        return false;
    }
    
    // Escribir cada perfil en una línea
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
