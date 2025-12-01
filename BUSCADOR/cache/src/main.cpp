#include "../include/cache_server.h"
#include <iostream>
#include <cstdlib>
#include <unistd.h>

using namespace std;

int main() {
    cout << "=================================================" << endl;
    cout << "          SERVIDOR CACHE - SISTOPE               " << endl;
    cout << "=================================================" << endl;
    cout << "[CACHE] PID: " << getpid() << endl;
    
    // Leer configuración desde variables de entorno
    const char* portStr = getenv("CACHE_PORT");
    const char* sizeStr = getenv("CACHE_SIZE");
    const char* motorHost = getenv("MOTOR_HOST");
    const char* motorPortStr = getenv("MOTOR_PORT");
    
    // Valores por defecto
    int puerto = portStr ? atoi(portStr) : 8080;
    int tamanoCache = sizeStr ? atoi(sizeStr) : 100;
    string host = motorHost ? motorHost : "127.0.0.1";
    int motorPort = motorPortStr ? atoi(motorPortStr) : 8081;
    
    cout << "[CONFIG] Puerto CACHE: " << puerto << endl;
    cout << "[CONFIG] Tamaño cache: " << tamanoCache << endl;
    cout << "[CONFIG] Motor: " << host << ":" << motorPort << endl;
    cout << "=================================================" << endl;
    
    // Crear e iniciar servidor
    CacheServer servidor(puerto, tamanoCache, host, motorPort);
    servidor.iniciar();
    
    return 0;
}
