#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <cctype>

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


void ingresarUsuario() {
    string filePath = leerVariableEnv("USERS_FILE");
    if (filePath.empty()) {
        cerr << "[ingresarUsuario] No se encontró 'USERS_FILE' en '.env'." << endl;
        return;
    }

    string id, nombre, username, password, perfil;

    cout << "ID: ";
    getline(cin >> ws, id);  // consume '\n' pendiente
    trim(id);
    cout << "Nombre: ";
    getline(cin, nombre);
    trim(nombre);
    cout << "Username: ";
    getline(cin, username);
    trim(username);
    cout << "Contraseña: ";
    getline(cin, password);
    trim(password);
    cout << "Perfil: ";
    getline(cin, perfil);
    trim(perfil);

    // Valida campos obligatorios que 'listarUsuarios' exige
    if (id.empty() || nombre.empty() || perfil.empty()) {
        cerr << "[ingresarUsuario] Campos obligatorios vacíos (id, nombre, perfil)." << endl;
        return;
    }

    ofstream archivo(filePath, ios::app);
    if (!archivo.is_open()) {
        cerr << "[ingresarUsuario] No se pudo abrir: " << filePath << endl;
        return;
    }

    archivo << id << "," << nombre << "," << username << "," << password << "," << perfil << "\n";
    archivo.flush();

    cout << "Usuario ingresado correctamente en " << filePath << "." << endl;
}



// Lista usuarios leyendo 'USERS_FILE' y esperando líneas: id,nombre,username,password,perfil
void listarUsuarios() {
    string filePath = leerVariableEnv("USERS_FILE");
    if (filePath.empty()) {
        cerr << "[listarUsuarios] No se encontró 'USERS_FILE' en '.env'." << endl;
        return;
    }

    ifstream archivo(filePath);
    if (!archivo.is_open()) {
        cerr << "[listarUsuarios] No se pudo abrir: " << filePath << endl;
        return;
    }

    cout << "ID\tNombre\tPerfil" << endl;
    cout << "-----------------------" << endl;

    string linea;
    while (getline(archivo, linea)) {
        trim(linea);
        if (linea.empty() || linea[0] == '#') continue;

        string id, nombre, username, password, perfil;
        stringstream datos(linea);

        getline(datos, id, ',');
        trim(id);
        getline(datos, nombre, ',');
        trim(nombre);
        getline(datos, username, ',');
        trim(username);
        getline(datos, password, ',');
        trim(password);
        getline(datos, perfil, ',');
        trim(perfil);

        if (id.empty() || nombre.empty() || perfil.empty()) continue;

        cout << id << "\t" << nombre << "\t" << perfil << endl;
    }
}


void eliminarUsuario() {
    string filePath = leerVariableEnv("USERS_FILE");
    if (filePath.empty()) {
        cerr << "[eliminarUsuario] No se encontró 'USERS_FILE' en '.env'." << endl;
        return;
    }

    string targetId;
    cout << "ID a eliminar (deje vacío o escriba 'c' para cancelar): ";
    if (!getline(cin, targetId)) {
        cerr << "[eliminarUsuario] Error de entrada." << endl;
        return;
    }
    trim(targetId);
    if (targetId.empty() || targetId == "c" || targetId == "C") {
        cout << "Operación cancelada." << endl;
        return;
    }

    ifstream in(filePath);
    if (!in.is_open()) {
        cerr << "[eliminarUsuario] No se pudo abrir: " << filePath << endl;
        return;
    }

    size_t removedCount = 0;
    string outContent, line;

    while (getline(in, line)) {
        string s = line;
        trim(s);
        if (s.empty() || s[0] == '#') {
            outContent += line;
            outContent.push_back('\n');
            continue;
        }

        string idField;
        stringstream ss(line);
        getline(ss, idField, ',');
        trim(idField);

        if (!idField.empty() && idField == targetId) {
            ++removedCount; // omite la línea (elimina)
            continue;
        }

        outContent += line;
        outContent.push_back('\n');
    }
    in.close();

    if (removedCount == 0) {
        cout << "No se encontró usuario con ID " << targetId << "." << endl;
        return;
    }

    cout << "Se eliminarán " << removedCount << " registro(s) con ID " << targetId << ". Confirmar? (s/N): ";
    string resp;
    if (!getline(cin, resp)) {
        cerr << "[eliminarUsuario] Error de entrada." << endl;
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
        cerr << "[eliminarUsuario] No se pudo abrir para escribir: " << filePath << endl;
        return;
    }
    out << outContent;
    out.flush();

    cout << "Eliminado(s) " << removedCount << " registro(s) con ID " << targetId << "." << endl;
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
