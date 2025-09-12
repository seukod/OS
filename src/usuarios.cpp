// Implementación de funciones para gestión de usuarios y perfiles en memoria
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <cctype>
#include "../include/users_auth.h"
#include "../include/utils/string_utils.h"

using namespace std;

// Declaraciones adelantadas de funciones auxiliares
int contarUsuariosConPerfil(const string& perfil);
bool esPerfilProtegido(const string& nombrePerfil);

// Obtiene el siguiente ID disponible para un usuario leyendo el archivo y encontrando el ID máximo
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

// Verifica si un archivo termina con salto de línea
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


// Lista todos los usuarios cargados en memoria mostrando ID, nombre y perfil
void listarUsuarios() {
    if (g_usuarios.empty()) {
        cerr << "[listarUsuarios] No hay usuarios cargados en memoria." << endl;
        return;
    }

    cout << "ID\tNombre\t\tPerfil" << endl;
    cout << "--------------------------------" << endl;

    for (const auto& u : g_usuarios) {
        cout << u.id << "\t" << u.nombre << "\t" << u.perfil << endl;
    }
}

// Verifica si una cadena contiene solo dígitos
static inline bool allDigits(const string& s) {
    if (s.empty()) return false;
    for (unsigned char c : s) if (!isdigit(c)) return false;
    return true;
}

// Normaliza funcionalidades: quita espacios, valida que cada token sea numérico y las une con comas
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

// Obtiene el siguiente ID de usuario disponible basándose en los usuarios en memoria
static int getNextUserIdFromMemory() {
    int maxId = 0;
    for (const auto& usuario : g_usuarios) {
        if (usuario.id > maxId) {
            maxId = usuario.id;
        }
    }
    return maxId + 1;
}

// Permite ingresar un nuevo usuario solicitando datos por consola y lo agrega a la memoria
void ingresarUsuarioEnMemoria() {
    int id = getNextUserIdFromMemory();

    string nombre, username, password, perfil;
    cout << "Nombre: ";
    if (!getline(cin >> ws, nombre)) { 
        cerr << "[ingresarUsuarioEnMemoria] Error de entrada." << endl; 
        return; 
    }
    trim(nombre);

    cout << "Username: ";
    if (!getline(cin, username)) { 
        cerr << "[ingresarUsuarioEnMemoria] Error de entrada." << endl; 
        return; 
    }
    trim(username);

    cout << "Contraseña: ";
    if (!getline(cin, password)) { 
        cerr << "[ingresarUsuarioEnMemoria] Error de entrada." << endl; 
        return; 
    }
    trim(password);

    cout << "Perfil: ";
    if (!getline(cin, perfil)) { 
        cerr << "[ingresarUsuarioEnMemoria] Error de entrada." << endl; 
        return; 
    }
    trim(perfil);

    if (nombre.empty() || perfil.empty()) {
        cerr << "[ingresarUsuarioEnMemoria] Campos obligatorios vacíos (nombre, perfil)." << endl;
        return;
    }

    // Verificar si el username ya existe
    for (const auto& u : g_usuarios) {
        if (u.username == username) {
            cerr << "[ingresarUsuarioEnMemoria] Ya existe un usuario con ese username." << endl;
            return;
        }
    }

    // Crear nuevo usuario y agregarlo al vector en memoria
    Usuario nuevoUsuario;
    nuevoUsuario.id = id;
    nuevoUsuario.nombre = nombre;
    nuevoUsuario.username = username;
    nuevoUsuario.password = password;
    nuevoUsuario.perfil = perfil;

    g_usuarios.push_back(nuevoUsuario);
    cout << "Usuario ingresado en memoria con ID " << id << ". Use 'Guardar Cambios' para persistir." << endl;
}

// Elimina un usuario de la memoria basándose en su ID, con validaciones de seguridad
void eliminarUsuarioEnMemoria() {
    string target;
    cout << "ID a eliminar (vacío o 'c' para cancelar): ";
    if (!getline(cin >> ws, target)) { 
        cerr << "[eliminarUsuarioEnMemoria] Error de entrada." << endl; 
        return; 
    }
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
        cerr << "[eliminarUsuarioEnMemoria] ID inválido." << endl;
        return;
    }

    // Buscar usuario en memoria para validaciones
    Usuario* usuarioAEliminar = nullptr;
    for (auto& usuario : g_usuarios) {
        if (usuario.id == targetId) {
            usuarioAEliminar = &usuario;
            break;
        }
    }

    if (usuarioAEliminar == nullptr) {
        cout << "No se encontró usuario con ID " << targetId << "." << endl;
        return;
    }

    // Validación: No permitir eliminar usuarios admin si quedaría menos de 1
    if (usuarioAEliminar->perfil == "admin") {
        int adminCount = contarUsuariosConPerfil("admin");
        if (adminCount <= 1) {
            cout << "ERROR: No se puede eliminar el último usuario administrador." << endl;
            cout << "Debe existir al menos un administrador en el sistema." << endl;
            return;
        }
        
        cout << "ADVERTENCIA: Está eliminando un usuario administrador." << endl;
        cout << "Quedarán " << (adminCount - 1) << " administrador(es) en el sistema." << endl;
        cout << "¿Está seguro? (s/N): ";
        string confirmacion;
        getline(cin, confirmacion);
        trim(confirmacion);
        if (confirmacion != "s" && confirmacion != "S") {
            cout << "Operación cancelada." << endl;
            return;
        }
    }

    // Buscar y eliminar usuario en memoria
    auto it = g_usuarios.begin();
    int encontrados = 0;
    while (it != g_usuarios.end()) {
        if (it->id == targetId) {
            encontrados++;
            it = g_usuarios.erase(it);
        } else {
            ++it;
        }
    }

    if (encontrados == 0) {
        cout << "No se encontró usuario con ID " << targetId << "." << endl;
        return;
    }

    cout << "Eliminado(s) " << encontrados << " usuario(s) de memoria. Use 'Guardar Cambios' para persistir." << endl;
}

// Permite ingresar un nuevo perfil solicitando datos por consola y lo agrega a la memoria
void ingresarPerfilEnMemoria() {
    string nombre, funcsRaw;
    cout << "Nombre del perfil (deje vacío o escriba 'c' para cancelar): ";
    if (!getline(cin >> ws, nombre)) {
        cerr << "[ingresarPerfilEnMemoria] Error de entrada." << endl;
        return;
    }
    trim(nombre);
    if (nombre.empty() || nombre == "c" || nombre == "C") {
        cout << "Operación cancelada." << endl;
        return;
    }

    cout << "Funcionalidades separadas por coma (p. ej., 0,1,3). Vacío para cancelar: ";
    if (!getline(cin, funcsRaw)) {
        cerr << "[ingresarPerfilEnMemoria] Error de entrada." << endl;
        return;
    }
    trim(funcsRaw);
    if (funcsRaw.empty()) {
        cout << "Operación cancelada." << endl;
        return;
    }

    string funcs = normalizaFuncs(funcsRaw);
    if (funcs.empty()) {
        cerr << "[ingresarPerfilEnMemoria] Lista de funcionalidades inválida. Use sólo números separados por coma." << endl;
        return;
    }

    // Verificar si ya existe un perfil con ese nombre en memoria
    for (const auto& p : g_perfiles) {
        if (p.nombre == nombre) {
            cerr << "[ingresarPerfilEnMemoria] Ya existe un perfil con ese nombre en memoria." << endl;
            return;
        }
    }

    // Crear nuevo perfil y agregarlo al vector en memoria
    Perfil nuevoPerfil;
    nuevoPerfil.nombre = nombre;
    
    // Parsear permisos
    stringstream ss(funcs);
    string permiso;
    while (getline(ss, permiso, ',')) {
        try {
            nuevoPerfil.permisos.push_back(stoi(permiso));
        } catch (...) {
            cerr << "[ingresarPerfilEnMemoria] Permiso inválido: " << permiso << endl;
        }
    }

    g_perfiles.push_back(nuevoPerfil);
    cout << "Perfil ingresado en memoria. Use 'Guardar Cambios' para persistir." << endl;
}

// Elimina un perfil de la memoria basándose en su nombre, con validaciones de seguridad
void eliminarPerfilEnMemoria() {
    string targetName;
    cout << "Nombre del perfil a eliminar (deje vacío o escriba 'c' para cancelar): ";
    if (!getline(cin >> ws, targetName)) {
        cerr << "[eliminarPerfilEnMemoria] Error de entrada." << endl;
        return;
    }
    trim(targetName);
    if (targetName.empty() || targetName == "c" || targetName == "C") {
        cout << "Operación cancelada." << endl;
        return;
    }

    // Validación: No permitir eliminar perfiles protegidos
    if (esPerfilProtegido(targetName)) {
        cout << "ERROR: No se puede eliminar el perfil '" << targetName << "'." << endl;
        cout << "Este perfil está protegido y es esencial para el funcionamiento del sistema." << endl;
        return;
    }

    // Validación: Verificar si hay usuarios que usan este perfil
    int usuariosConPerfil = contarUsuariosConPerfil(targetName);
    if (usuariosConPerfil > 0) {
        cout << "ADVERTENCIA: Hay " << usuariosConPerfil << " usuario(s) que usan el perfil '" << targetName << "'." << endl;
        cout << "Si elimina este perfil, esos usuarios podrían quedar sin acceso." << endl;
        cout << "¿Está seguro de que desea continuar? (s/N): ";
        string confirmacion;
        getline(cin, confirmacion);
        trim(confirmacion);
        if (confirmacion != "s" && confirmacion != "S") {
            cout << "Operación cancelada." << endl;
            return;
        }
    }

    // Buscar y eliminar perfil de memoria
    auto it = g_perfiles.begin();
    int removedCount = 0;
    while (it != g_perfiles.end()) {
        if (it->nombre == targetName) {
            it = g_perfiles.erase(it);
            removedCount++;
        } else {
            ++it;
        }
    }

    if (removedCount == 0) {
        cout << "No se encontró perfil con nombre " << targetName << " en memoria." << endl;
        return;
    }

    cout << "Eliminado(s) " << removedCount << " perfil(es) de memoria. Use 'Guardar Cambios' para persistir." << endl;
}

// Lista todos los perfiles cargados en memoria con sus respectivos permisos
void listarPerfilesEnMemoria() {
    if (g_perfiles.empty()) {
        cout << "No hay perfiles cargados en memoria." << endl;
        return;
    }

    cout << "Perfiles en memoria:" << endl;
    cout << "--------------------" << endl;

    for (const auto& perfil : g_perfiles) {
        cout << "- " << perfil.nombre << ";";
        for (size_t i = 0; i < perfil.permisos.size(); i++) {
            cout << perfil.permisos[i];
            if (i < perfil.permisos.size() - 1) {
                cout << ",";
            }
        }
        cout << endl;
    }
}

// Cuenta cuántos usuarios en memoria tienen un perfil específico
int contarUsuariosConPerfil(const string& perfil) {
    int count = 0;
    for (const auto& usuario : g_usuarios) {
        if (usuario.perfil == perfil) {
            count++;
        }
    }
    return count;
}

// Verifica si un perfil está en la lista de perfiles protegidos (no se pueden eliminar)
bool esPerfilProtegido(const string& nombrePerfil) {
    // Lista de perfiles protegidos que no se pueden eliminar
    vector<string> perfilesProtegidos = {"admin", "administrador", "root", "system"};
    
    for (const string& protegido : perfilesProtegidos) {
        if (nombrePerfil == protegido) {
            return true;
        }
    }
    return false;
}
