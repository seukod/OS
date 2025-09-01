#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <cctype>
#include "centro.h"
#include <algorithm>
#include <limits>

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
string leerVariableEnv(const string &nombreVariable, const string &archivoEnv = ".env") {
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

static int getNextUserId(const string& filePath) {
    ifstream in(filePath);
    int maxId = 0;
    if (in.is_open()) {
        string line;
        while (getline(in, line)) {
            string s = line; trim(s);
            if (s.empty() || s[0] == '#') continue;
            string idField;
            stringstream ss(line);
            getline(ss, idField, ',');
            trim(idField);
            if (idField.empty()) continue;
            try {
                int id = stoi(idField);
                if (id > maxId) maxId = id;
            } catch (...) { /* ignora IDs no numéricos */ }
        }
    }
    return maxId + 1;
}
static bool fileEndsWithNewline(const string& path) {
    ifstream in(path, ios::binary);
    if (!in.is_open()) return false;
    in.seekg(0, ios::end);
    auto size = in.tellg();
    if (size <= 0) return false;
    in.seekg(-1, ios::end);
    char last = '\n';
    in.read(&last, 1);
    return last == '\n';
}

void ingresarUsuario(vector<Usuario>& usuarios, const string& filePath) {
    string nombre, username, password, perfil;

    cout << "Nombre: ";
    if (!getline(cin >> ws, nombre)) {
        cerr << "[ingresarUsuario] Error de entrada." << endl;
        return;
    }
    trim(nombre);

    cout << "Username: ";
    if (!getline(cin, username)) {
        cerr << "[ingresarUsuario] Error de entrada." << endl;
        return;
    }
    trim(username);

    cout << "Contraseña: ";
    if (!getline(cin, password)) {
        cerr << "[ingresarUsuario] Error de entrada." << endl;
        return;
    }
    trim(password);

    cout << "Perfil: ";
    if (!getline(cin, perfil)) {
        cerr << "[ingresarUsuario] Error de entrada." << endl;
        return;
    }
    trim(perfil);

    if (nombre.empty() || perfil.empty()) {
        cerr << "[ingresarUsuario] Campos obligatorios vacíos (nombre, perfil)." << endl;
        return;
    }

    // Generar ID automáticamente (tomando el siguiente disponible)
    int id = 1;
    if (!usuarios.empty()) {
        id = usuarios.back().id + 1;
    }

    Usuario nuevo;
    nuevo.id = id;
    nuevo.nombre = nombre;
    nuevo.username = username;
    nuevo.password = password;
    nuevo.perfil = perfil;

    // Confirmar antes de guardar
    int opcion;
    cout << "\n¿Desea guardar los cambios realizados? (1 = Sí, 0 = No): ";
    cin >> opcion;

    if (opcion == 1) {
        usuarios.push_back(nuevo);
        guardarCambios(usuarios, filePath);
        cout << "Usuario agregado correctamente con ID " << id << ".\n";
    } else {
        cout << "Operación cancelada. No se guardaron cambios.\n";
    }


}


void eliminarUsuario(vector<Usuario>& usuarios, const string& filePath) {
    string target;
    cout << "ID a eliminar (vacío o 'c' para cancelar): ";
    if (!getline(cin >> ws, target)) { cerr << "[eliminarUsuario] Error de entrada." << endl; return; }
    trim(target);
    if (target.empty() || target == "c" || target == "C") {
        cout << "Operación cancelada." << endl;
        return;
    }

    int targetId = -1;
    try {
        targetId = stoi(target);
        if (targetId < 0) throw std::invalid_argument("negativo");
    } catch (...) {
        cerr << "[eliminarUsuario] ID inválido." << endl;
        return;
    }

    // Buscar usuarios a eliminar en el vector
    auto it = std::find_if(usuarios.begin(), usuarios.end(), [&](const Usuario& u) {
        return u.id == targetId;
    });

    if (it == usuarios.end()) {
        cout << "No se encontró usuario con ID " << targetId << "." << endl;
        return;
    }

    // Preguntar confirmación
    cout << "¿Desea eliminar al usuario con ID " << targetId << "? (1 = Sí, 0 = No): ";
    int opcion;
    cin >> opcion;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (opcion == 1) {
        // Eliminar del vector
        usuarios.erase(it);
        // Guardar cambios en el archivo
        guardarCambios(usuarios, filePath);
        cout << "Usuario con ID " << targetId << " eliminado y cambios guardados.\n";
    } else {
        cout << "Operación cancelada. No se realizaron cambios.\n";
    }
}



// Lista usuarios leyendo 'USERS_FILE' y esperando líneas: id,nombre,username,password,perfil
void listarUsuarios(vector<Usuario>& usuarios) {
    if (usuarios.empty()) {
        cerr << "[listarUsuarios] No hay usuarios cargados." << endl;
        return;
    }

    cout << "ID\tNombre\tPerfil" << endl;
    cout << "-----------------------" << endl;

    for (const auto& u : usuarios) {
        if (u.id <= 0 || u.nombre.empty() || u.perfil.empty()) continue;
        cout << u.id << "\t" << u.nombre << "\t" << u.perfil << endl;
    }
}



// aqui van los perfiles
// brigido

// Normaliza "funcionalidades": quita espacios, valida que cada token sea numérico y las une con comas.
static inline bool allDigits(const string& s) {
    if (s.empty()) return false;
    for (unsigned char c : s) if (!isdigit(c)) return false;
    return true;
}

static inline string normalizaFuncs(const string& raw) {
    string out;
    string token;
    stringstream ss(raw);
    bool first = true;
    while (getline(ss, token, ',')) {
        trim(token);
        if (token.empty()) continue;
        if (!allDigits(token)) return ""; // inválido si hay algo no numérico
        if (!first) out.push_back(',');
        out += token;
        first = false;
    }
    return out; // puede quedar vacío si no hubo tokens válidos
}


void ingresarPerfil() {
    string filePath = leerVariableEnv("PROFILES_FILE");
    if (filePath.empty()) {
        cerr << "[ingresarPerfil] No se encontró 'PROFILES_FILE' en '.env'." << endl;
        return;
    }

    string nombre, funcsRaw;
    cout << "Nombre del perfil (deje vacío o escriba 'c' para cancelar): ";
    if (!getline(cin >> ws, nombre)) {
        cerr << "[ingresarPerfil] Error de entrada." << endl;
        return;
    }
    trim(nombre);
    if (nombre.empty() || nombre == "c" || nombre == "C") {
        cout << "Operación cancelada." << endl;
        return;
    }

    cout << "Funcionalidades separadas por coma (p. ej., 0,1,3). Vacío para cancelar: ";
    if (!getline(cin, funcsRaw)) {
        cerr << "[ingresarPerfil] Error de entrada." << endl;
        return;
    }
    trim(funcsRaw);
    if (funcsRaw.empty()) {
        cout << "Operación cancelada." << endl;
        return;
    }

    string funcs = normalizaFuncs(funcsRaw);
    if (funcs.empty()) {
        cerr << "[ingresarPerfil] Lista de funcionalidades inválida. Use sólo números separados por coma." << endl;
        return;
    }

    // Evita duplicar nombre de perfil
    {
        ifstream in(filePath);
        if (in.is_open()) {
            string line;
            while (getline(in, line)) {
                string s = line; trim(s);
                if (s.empty() || s[0] == '#') continue;
                size_t pos = s.find(';');
                string nom = (pos == string::npos) ? s : s.substr(0, pos);
                trim(nom);
                if (nom == nombre) {
                    cerr << "[ingresarPerfil] Ya existe un perfil con ese nombre." << endl;
                    return;
                }
            }
        }
    }

    // Si el archivo no termina en '\n', agrega uno antes de appender
    bool needsLeadingNewline = false;
    {
        ifstream in(filePath, ios::binary);
        if (in.is_open()) {
            in.seekg(0, ios::end);
            auto size = in.tellg();
            if (size > 0) {
                in.seekg(-1, ios::end);
                char last = '\n';
                in.read(&last, 1);
                if (last != '\n') needsLeadingNewline = true;
            }
        }
    }

    ofstream out(filePath, ios::app | ios::binary);
    if (!out.is_open()) {
        cerr << "[ingresarPerfil] No se pudo abrir: " << filePath << endl;
        return;
    }

    if (needsLeadingNewline) out << '\n';
    out << nombre << ";" << funcs << '\n';
    out.flush();

    cout << "Perfil ingresado correctamente en " << filePath << "." << endl;
}

void eliminarPerfil() {
    string filePath = leerVariableEnv("PROFILES_FILE");
    if (filePath.empty()) {
        cerr << "[eliminarPerfil] No se encontró 'PROFILES_FILE' en '.env'." << endl;
        return;
    }

    string targetName;
    cout << "Nombre del perfil a eliminar (deje vacío o escriba 'c' para cancelar): ";
    if (!getline(cin >> ws, targetName)) {
        cerr << "[eliminarPerfil] Error de entrada." << endl;
        return;
    }
    trim(targetName);
    if (targetName.empty() || targetName == "c" || targetName == "C") {
        cout << "Operación cancelada." << endl;
        return;
    }

    ifstream in(filePath);
    if (!in.is_open()) {
        cerr << "[eliminarPerfil] No se pudo abrir: " << filePath << endl;
        return;
    }

    size_t removedCount = 0;
    string outContent, line;
    while (getline(in, line)) {
        string s = line; trim(s);
        if (s.empty() || s[0] == '#') {
            outContent += line; outContent.push_back('\n');
            continue;
        }
        size_t pos = s.find(';');
        string nom = (pos == string::npos) ? s : s.substr(0, pos);
        trim(nom);

        if (!nom.empty() && nom == targetName) {
            ++removedCount; // omitir
            continue;
        }
        outContent += line; outContent.push_back('\n');
    }
    in.close();

    if (removedCount == 0) {
        cout << "No se encontró perfil con nombre " << targetName << "." << endl;
        return;
    }

    cout << "Se eliminarán " << removedCount << " registro(s) con nombre '" << targetName << "'. Confirmar? (s/N): ";
    string resp;
    if (!getline(cin, resp)) {
        cerr << "[eliminarPerfil] Error de entrada." << endl;
        return;
    }
    trim(resp);
    for (auto& ch : resp) ch = (char)tolower((unsigned char)ch);
    if (!(resp == "s" || resp == "si" || resp == "sí")) {
        cout << "Operación cancelada." << endl;
        return;
    }

    ofstream out(filePath, ios::trunc);
    if (!out.is_open()) {
        cerr << "[eliminarPerfil] No se pudo abrir para escribir: " << filePath << endl;
        return;
    }
    out << outContent;
    out.flush();
    cout << "Eliminado(s) " << removedCount << " perfil(es) con nombre " << targetName << "." << endl;
}




void listarPerfiles() {
    string filePath = leerVariableEnv("PROFILES_FILE");
    if (filePath.empty()) {
        cerr << "[listarPerfiles] No se encontró 'PROFILES_FILE' en '.env'." << endl;
        return;
    }

    ifstream archivo(filePath);
    if (!archivo.is_open()) {
        cerr << "[listarPerfiles] No se pudo abrir: " << filePath << endl;
        return;
    }

    cout << "Perfiles disponibles:" << endl;
    cout << "--------------------" << endl;

    string linea;
    while (getline(archivo, linea)) {
        trim(linea);
        if (linea.empty() || linea[0] == '#') continue;
        cout << "- " << linea << endl;
    }
}
