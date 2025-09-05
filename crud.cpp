#include "crud.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cctype>  // getenv
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
    // Obtener archivo desde .env
    string userFile = leerVariableEnv("USERS_FILE", ".env");
    vector<Usuario> usuarios;

    if (userFile.empty()) {
        cerr << "[ERROR] No se encontró USER_FILE en .env ni en variables de entorno." << endl;
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
void guardar_cambios(const vector<Usuario>& usuarios) {
    // Obtener la ruta del archivo desde .env
    string archivo = leerVariableEnv("USERS_FILE", ".env");
    if (archivo.empty()) {
        cerr << "[ERROR] No se encontró USERS_FILE en .env ni en variables de entorno." << endl;
        return;
    }

    ofstream out(archivo, ios::trunc);  // trunc borra todo el contenido anterior
    if (!out.is_open()) {
        cerr << "[ERROR] No se pudo abrir el archivo para guardar: " << archivo << endl;
        return;
    }

    for (const auto& u : usuarios) {
        out << u.id << ","
            << u.nombre << ","
            << u.username << ","
            << u.password << ","
            << u.perfil << "\n";
    }
    out.flush();
    cout << "Archivo '" << archivo << "' actualizado con " << usuarios.size() << " usuario(s)." << endl;
}