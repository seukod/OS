#include "../include/buscador.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

using namespace std;

Buscador::Buscador(const string& host, int port) 
    : cacheHost(host), cachePort(port), socketFd(-1) {
    cout << "[BUSCADOR] PID: " << getpid() << endl;
}

Buscador::~Buscador() {
    cerrarConexion();
}

void Buscador::cargarMapaLibros(const string& rutaMapa) {
    ifstream archivo(rutaMapa);
    if (!archivo.is_open()) {
        cerr << "[ERROR] No se pudo abrir el archivo de mapeo: " << rutaMapa << endl;
        return;
    }
    
    string linea;
    while (getline(archivo, linea)) {
        stringstream ss(linea);
        string idStr, nombre;
        
        if (getline(ss, idStr, ';') && getline(ss, nombre)) {
            try {
                int id = stoi(idStr);
                mapaLibros[id] = nombre;
            } catch (...) {
                cerr << "[ADVERTENCIA] Línea inválida en mapa: " << linea << endl;
            }
        }
    }
    
    archivo.close();
    cout << "[INFO] Mapa de libros cargado: " << mapaLibros.size() << " libros" << endl;
}

bool Buscador::validarIndiceInvertido(const string& rutaIndice) {
    ifstream archivo(rutaIndice);
    if (!archivo.is_open()) {
        cerr << "[ERROR] No existe el archivo de índice invertido: " << rutaIndice << endl;
        return false;
    }
    archivo.close();
    cout << "[INFO] Índice invertido encontrado: " << rutaIndice << endl;
    return true;
}

bool Buscador::conectarCache() {
    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd < 0) {
        cerr << "[ERROR] No se pudo crear el socket" << endl;
        return false;
    }
    
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(cachePort);
    
    if (inet_pton(AF_INET, cacheHost.c_str(), &serverAddr.sin_addr) <= 0) {
        cerr << "[ERROR] Dirección inválida: " << cacheHost << endl;
        return false;
    }
    
    if (connect(socketFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "[ERROR] No se pudo conectar al servidor CACHE en " 
             << cacheHost << ":" << cachePort << endl;
        return false;
    }
    
    cout << "[INFO] Conectado al servidor CACHE" << endl;
    return true;
}

void Buscador::cerrarConexion() {
    if (socketFd >= 0) {
        close(socketFd);
        socketFd = -1;
    }
}

string Buscador::enviarConsulta(const string& palabra) {
    // Enviar palabra al servidor
    if (send(socketFd, palabra.c_str(), palabra.length(), 0) < 0) {
        cerr << "[ERROR] Error al enviar consulta" << endl;
        return "";
    }
    
    // Recibir respuesta
    char buffer[4096] = {0};
    int bytesRecibidos = recv(socketFd, buffer, sizeof(buffer) - 1, 0);
    
    if (bytesRecibidos < 0) {
        cerr << "[ERROR] Error al recibir respuesta" << endl;
        return "";
    }
    
    return string(buffer, bytesRecibidos);
}

vector<ResultadoBusqueda> Buscador::parsearRespuestaJSON(const string& json) {
    vector<ResultadoBusqueda> resultados;
    
    // TODO: Implementar parser JSON completo
    // Por ahora, implementación simple
    // Formato esperado: {"palabra": "...", "resultados": [{"id": 1, "frecuencia": 10, "posiciones": [1,2,3]}, ...]}
    
    cout << "[DEBUG] JSON recibido: " << json << endl;
    
    // Placeholder: retornar resultados vacíos
    return resultados;
}

void Buscador::mostrarResultados(const vector<ResultadoBusqueda>& resultados, const string& palabra) {
    cout << "\n==================================================" << endl;
    cout << "           RESULTADOS DE BÚSQUEDA                 " << endl;
    cout << "==================================================" << endl;
    cout << "Palabra: " << palabra << endl;
    cout << "Total de resultados: " << resultados.size() << endl;
    cout << "==================================================" << endl;
    
    if (resultados.empty()) {
        cout << "No se encontraron resultados para '" << palabra << "'" << endl;
    } else {
        for (size_t i = 0; i < resultados.size(); i++) {
            const auto& res = resultados[i];
            cout << (i + 1) << ". " << res.libro << endl;
            cout << "   Frecuencia: " << res.frecuencia << " veces" << endl;
            cout << "   Primeras posiciones: ";
            
            size_t maxPosiciones = min(res.posiciones.size(), size_t(5));
            for (size_t j = 0; j < maxPosiciones; j++) {
                cout << res.posiciones[j];
                if (j < maxPosiciones - 1) cout << ", ";
            }
            if (res.posiciones.size() > 5) {
                cout << "...";
            }
            cout << endl << endl;
        }
    }
    
    cout << "==================================================" << endl;
}

void Buscador::buscar(const string& palabra) {
    cout << "\n[BUSCADOR] Buscando: '" << palabra << "'" << endl;
    
    // Conectar al servidor CACHE
    if (!conectarCache()) {
        cerr << "[ERROR] No se pudo establecer conexión con CACHE" << endl;
        return;
    }
    
    // Enviar consulta y recibir respuesta
    string respuestaJSON = enviarConsulta(palabra);
    
    if (respuestaJSON.empty()) {
        cerr << "[ERROR] No se recibió respuesta del servidor" << endl;
        cerrarConexion();
        return;
    }
    
    // Parsear respuesta JSON
    vector<ResultadoBusqueda> resultados = parsearRespuestaJSON(respuestaJSON);
    
    // Mapear IDs a nombres de libros
    for (auto& res : resultados) {
        // El parseo JSON debería llenar res.libro con el ID, 
        // aquí lo convertimos al nombre real
        // (implementación pendiente con el parser JSON completo)
    }
    
    // Mostrar resultados
    mostrarResultados(resultados, palabra);
    
    cerrarConexion();
}

void Buscador::iniciar() {
    cout << "\n==================================================" << endl;
    cout << "           BUSCADOR SISTOPE                       " << endl;
    cout << "==================================================" << endl;
    cout << "PID: " << getpid() << endl;
    cout << "==================================================" << endl;
    
    // Cargar mapa de libros
    cargarMapaLibros("../data/MAPA-LIBROS.csv");
    
    // Buscar archivos .idx disponibles en el directorio raíz del proyecto
    vector<string> archivosIdx;
    string comando = "find .. -maxdepth 1 -name '*.idx' -type f 2>/dev/null";
    
    FILE* pipe = popen(comando.c_str(), "r");
    if (pipe) {
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            string archivo = buffer;
            // Eliminar salto de línea
            if (!archivo.empty() && archivo[archivo.length()-1] == '\n') {
                archivo.erase(archivo.length()-1);
            }
            // Extraer solo el nombre del archivo sin la ruta
            size_t pos = archivo.find_last_of('/');
            if (pos != string::npos) {
                archivo = archivo.substr(pos + 1);
            }
            archivosIdx.push_back(archivo);
        }
        pclose(pipe);
    }
    
    // Verificar si hay archivos disponibles
    if (archivosIdx.empty()) {
        cerr << "\n[ERROR] No se encontraron archivos de índice invertido (.idx)" << endl;
        cerr << "Debe crear primero un índice invertido desde el menú principal." << endl;
        return;
    }
    
    // Mostrar archivos disponibles
    cout << "\n==================================================" << endl;
    cout << "(" << archivosIdx.size() << ") archivos .idx encontrados \n" << endl;
    cout << "Archivos de índice invertido disponibles:" << endl;
    cout << "==================================================" << endl;
    for (size_t i = 0; i < archivosIdx.size(); i++) {
        cout << (i + 1) << ". " << archivosIdx[i] << endl;
    }
    cout << "==================================================" << endl;
    
    // Solicitar nombre del archivo índice
    string archivoIndice;
    cout << "\nIngrese el nombre del archivo índice a usar: ";
    getline(cin, archivoIndice);
    
    if (archivoIndice.empty()) {
        cerr << "[ERROR] Debe ingresar el nombre del archivo índice" << endl;
        return;
    }
    
    // Si no termina en .idx, agregarlo
    if (archivoIndice.find(".idx") == string::npos) {
        archivoIndice += ".idx";
    }
    
    // Verificar que el archivo existe en la lista
    bool encontrado = false;
    for (const auto& archivo : archivosIdx) {
        if (archivo == archivoIndice) {
            encontrado = true;
            break;
        }
    }
    
    if (!encontrado) {
        cerr << "[ERROR] El archivo '" << archivoIndice << "' no está en la lista de archivos disponibles" << endl;
        return;
    }
    
    // Construir ruta completa
    string rutaCompleta = "../" + archivoIndice;
    ifstream testFile(rutaCompleta);
    if (!testFile.is_open()) {
        cerr << "[ERROR] No se pudo abrir el archivo: " << rutaCompleta << endl;
        return;
    }
    testFile.close();
    
    cout << "[INFO] Usando índice: " << archivoIndice << endl;
    
    // Solicitar palabra a buscar
    string palabra;
    cout << "\nIngrese la palabra a buscar: ";
    getline(cin, palabra);
    
    if (palabra.empty()) {
        cerr << "[ERROR] Debe ingresar una palabra" << endl;
        return;
    }
    
    // Realizar búsqueda
    buscar(palabra);
}
