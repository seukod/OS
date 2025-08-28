#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

// Función que carga variables del archivo .env
string leerVariableEnv(const string& nombreVariable, const string& archivoEnv = ".env") {
    ifstream envFile(archivoEnv);
    if (!envFile.is_open()) {
        cerr << "No se pudo abrir " << archivoEnv << endl;
        return "";
    }

    string linea;
    while (getline(envFile, linea)) {
        if (linea.empty() || linea[0] == '#') continue; // ignorar comentarios
        size_t pos = linea.find('=');
        if (pos == string::npos) continue;

        string key = linea.substr(0, pos);
        string value = linea.substr(pos + 1);

        if (key == nombreVariable) return value; // devolvemos el valor buscado
    }

    return ""; // no encontrado
}

// Función listarUsuarios usando la variable USER_FILE
void listarUsuarios() {
    string filePath = leerVariableEnv("USER_FILE");
    ifstream archivo(filePath);

    string linea;
    cout << "ID\tNombre\tPerfil" << endl;
    cout << "-----------------------" << endl;

    while (getline(archivo, linea, ';')) {
        if (linea.empty()) continue;

        stringstream datos(linea);
        string id, nombre, username, password, perfil;

        getline(datos, id, ',');
        getline(datos, nombre, ',');
        getline(datos, username, ',');
        getline(datos, password, ',');
        getline(datos, perfil, ',');

        cout << id << "\t" << nombre << "\t" << perfil;
    }

    archivo.close();
}


