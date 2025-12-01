#include "../include/cache_server.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <sstream>
#include <list>

using namespace std;

CacheServer::CacheServer(int puerto, int tamanoCache, const string& motorHost, int motorPort)
    : puerto(puerto), tamanoCache(tamanoCache), motorHost(motorHost), 
      motorPort(motorPort), socketServidor(-1) {
    cout << "[CACHE] Servidor inicializado" << endl;
    cout << "[CACHE] Puerto: " << puerto << endl;
    cout << "[CACHE] Tamaño cache: " << tamanoCache << endl;
    cout << "[CACHE] Motor: " << motorHost << ":" << motorPort << endl;
}

CacheServer::~CacheServer() {
    detener();
}

bool CacheServer::inicializarSocket() {
    socketServidor = socket(AF_INET, SOCK_STREAM, 0);
    if (socketServidor < 0) {
        cerr << "[ERROR] No se pudo crear el socket" << endl;
        return false;
    }
    
    // Permitir reutilizar el puerto
    int opt = 1;
    if (setsockopt(socketServidor, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        cerr << "[ADVERTENCIA] No se pudo configurar SO_REUSEADDR" << endl;
    }
    
    struct sockaddr_in direccion;
    direccion.sin_family = AF_INET;
    direccion.sin_addr.s_addr = INADDR_ANY;
    direccion.sin_port = htons(puerto);
    
    if (bind(socketServidor, (struct sockaddr*)&direccion, sizeof(direccion)) < 0) {
        cerr << "[ERROR] No se pudo hacer bind en el puerto " << puerto << endl;
        close(socketServidor);
        return false;
    }
    
    if (listen(socketServidor, 10) < 0) {
        cerr << "[ERROR] Error al poner el socket en modo escucha" << endl;
        close(socketServidor);
        return false;
    }
    
    cout << "[INFO] Socket iniciado en puerto " << puerto << endl;
    return true;
}

string CacheServer::obtenerDeCache(const string& clave) {
    lock_guard<mutex> lock(cacheMutex);
    
    auto it = cache.find(clave);
    if (it != cache.end()) {
        cout << "[HIT] Palabra '" << clave << "' encontrada en cache" << endl;
        // Mover la clave al frente de la lista para marcarla como recién usada
        ordenUso.erase(it->second.second);
        ordenUso.push_front(clave);
        it->second.second = ordenUso.begin();
        return it->second.first;
    }
    
    cout << "[MISS] Palabra '" << clave << "' no está en cache" << endl;
    return "";
}

void CacheServer::agregarACache(const string& clave, const string& valor) {
    lock_guard<mutex> lock(cacheMutex);
    
    // Si ya existe, no agregar de nuevo (solo actualizar)
    if (cache.find(clave) != cache.end()) {
        // Actualizar valor y mover al frente
        auto it = cache.find(clave);
        it->second.first = valor;
        ordenUso.erase(it->second.second);
        ordenUso.push_front(clave);
        it->second.second = ordenUso.begin();
        return;
    }
    
    // Si el cache está lleno, eliminar el menos recién usado (el último de la lista)
    if (cache.size() >= (size_t)tamanoCache) {
        string claveAntigua = ordenUso.back();
        ordenUso.pop_back();
        cache.erase(claveAntigua);
        cout << "[CACHE] Eliminado: '" << claveAntigua << "' (LRU)" << endl;
    }
    
    // Agregar nueva entrada al frente
    ordenUso.push_front(clave);
    cache[clave] = {valor, ordenUso.begin()};
    cout << "[CACHE] Agregado: '" << clave << "' (total: " << cache.size() << "/" << tamanoCache << ")" << endl;
}

string CacheServer::consultarMotor(const string& palabra, const string& archivoIndice) {
    cout << "[CACHE] Consultando MOTOR_BUSQUEDA para '" << palabra << "'" << endl;
    
    // Crear socket para conectar al motor
    int socketMotor = socket(AF_INET, SOCK_STREAM, 0);
    if (socketMotor < 0) {
        cerr << "[ERROR] No se pudo crear socket para motor" << endl;
        return "{\"error\": \"No se pudo conectar al motor\"}";
    }
    
    struct sockaddr_in direccionMotor;
    direccionMotor.sin_family = AF_INET;
    direccionMotor.sin_port = htons(motorPort);
    
    if (inet_pton(AF_INET, motorHost.c_str(), &direccionMotor.sin_addr) <= 0) {
        cerr << "[ERROR] Dirección del motor inválida" << endl;
        close(socketMotor);
        return "{\"error\": \"Dirección del motor inválida\"}";
    }
    
    if (connect(socketMotor, (struct sockaddr*)&direccionMotor, sizeof(direccionMotor)) < 0) {
        cerr << "[ERROR] No se pudo conectar al MOTOR_BUSQUEDA en " 
             << motorHost << ":" << motorPort << endl;
        close(socketMotor);
        return "{\"error\": \"Motor de búsqueda no disponible\"}";
    }
    
    // Enviar consulta al motor (formato: palabra|archivo)
    string consulta = palabra + "|" + archivoIndice;
    if (send(socketMotor, consulta.c_str(), consulta.length(), 0) < 0) {
        cerr << "[ERROR] Error al enviar consulta al motor" << endl;
        close(socketMotor);
        return "{\"error\": \"Error al enviar consulta\"}";
    }
    
    // Recibir respuesta del motor
    char buffer[8192] = {0};
    int bytesRecibidos = recv(socketMotor, buffer, sizeof(buffer) - 1, 0);
    close(socketMotor);
    
    if (bytesRecibidos <= 0) {
        cerr << "[ERROR] Error al recibir respuesta del motor" << endl;
        return "{\"error\": \"Error al recibir respuesta del motor\"}";
    }
    
    string respuesta(buffer, bytesRecibidos);
    cout << "[INFO] Respuesta del motor recibida (" << bytesRecibidos << " bytes)" << endl;
    
    return respuesta;
}

void CacheServer::manejarCliente(int socketCliente) {
    char buffer[1024] = {0};
    int bytesRecibidos = recv(socketCliente, buffer, sizeof(buffer) - 1, 0);
    
    if (bytesRecibidos <= 0) {
        close(socketCliente);
        return;
    }
    
    string consulta(buffer, bytesRecibidos);
    cout << "\n[CLIENTE] Consulta recibida: '" << consulta << "'" << endl;
    
    // Parsear consulta (formato esperado: palabra|archivo.idx)
    size_t separador = consulta.find('|');
    string palabra, archivoIndice;
    
    if (separador != string::npos) {
        palabra = consulta.substr(0, separador);
        archivoIndice = consulta.substr(separador + 1);
    } else {
        // Si no hay separador, asumir que solo es la palabra
        palabra = consulta;
        archivoIndice = "default.idx";
    }
    
    // Crear clave de cache (palabra + archivo)
    string clave = palabra + "@" + archivoIndice;
    
    // Buscar en cache
    string respuesta = obtenerDeCache(clave);
    
    // Si no está en cache, consultar al motor
    if (respuesta.empty()) {
        respuesta = consultarMotor(palabra, archivoIndice);
        
        // Agregar a cache si la respuesta es válida
        if (!respuesta.empty() && respuesta.find("error") == string::npos) {
            agregarACache(clave, respuesta);
        }
    }
    
    // Enviar respuesta al cliente
    send(socketCliente, respuesta.c_str(), respuesta.length(), 0);
    cout << "[INFO] Respuesta enviada al cliente" << endl;
    
    close(socketCliente);
}

void CacheServer::iniciar() {
    if (!inicializarSocket()) {
        cerr << "[ERROR] No se pudo inicializar el servidor" << endl;
        return;
    }
    
    cout << "[INFO] Servidor CACHE escuchando en puerto " << puerto << endl;
    cout << "[INFO] Esperando conexiones..." << endl;
    cout << "[INFO] Presione Ctrl+C para detener el servidor" << endl;
    
    while (true) {
        struct sockaddr_in direccionCliente;
        socklen_t tamañoDireccion = sizeof(direccionCliente);
        
        int socketCliente = accept(socketServidor, 
                                   (struct sockaddr*)&direccionCliente, 
                                   &tamañoDireccion);
        
        if (socketCliente < 0) {
            cerr << "[ERROR] Error al aceptar conexión" << endl;
            continue;
        }
        
        char ipCliente[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &direccionCliente.sin_addr, ipCliente, INET_ADDRSTRLEN);
        cout << "\n[CONEXION] Cliente conectado desde " << ipCliente 
             << ":" << ntohs(direccionCliente.sin_port) << endl;
        
        // Manejar cliente (en este caso de forma secuencial)
        // Para paralelismo, se podría usar threads o fork
        manejarCliente(socketCliente);
    }
}

void CacheServer::detener() {
    if (socketServidor >= 0) {
        close(socketServidor);
        socketServidor = -1;
        cout << "[INFO] Servidor CACHE detenido" << endl;
    }
}
