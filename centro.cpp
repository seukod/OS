#include "centro.h"
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

// Obtiene la ruta del archivo USUARIOS.txt desde .env
string getUserFilePath(const string& envFile) {
    ifstream env(envFile);
    if (!env.is_open()) {
        cerr << "Error al abrir el archivo .env" << endl;
        return "";
    }

    string line;
    while (getline(env, line)) {
        if (line.rfind("USERS_FILE=", 0) == 0) {
            return line.substr(11); // quitar "USERS_FILE="
        }
    }
    return "";
}

// Lee usuarios desde el archivo y los guarda en un vector dinámico
vector<Usuario> leerUsuarios(const string& filePath) {
    ifstream file(filePath);
    vector<Usuario> usuarios;

    if (!file.is_open()) {
        cerr << "Error al abrir archivo de usuarios: " << filePath << endl;
        return usuarios;
    }

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        Usuario u;

        ss >> u.id >> u.nombre >> u.username >> u.password >> u.perfil;

        if (!ss.fail()) {
            usuarios.push_back(u);
        }
    }

    return usuarios;
}
void guardarCambios(const vector<Usuario>& usuarios, const string& filePath) {
    ofstream out(filePath, ios::trunc); // trunc -> sobrescribe el archivo
    if (!out.is_open()) {
        cerr << "[guardarCambios] No se pudo abrir: " << filePath << endl;
        return;
    }

    for (const auto& u : usuarios) {
        out << u.id << " "
            << u.nombre << " "
            << u.username << " "
            << u.password << " "
            << u.perfil << "\n";
    }

    out.close();
    cout << "Cambios guardados correctamente en " << filePath << ".\n";
}