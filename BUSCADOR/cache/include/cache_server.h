#ifndef CACHE_SERVER_H
#define CACHE_SERVER_H

#include <string>
#include <unordered_map>
#include <list>
#include <utility>

using namespace std;

class CacheServer {
private:
    int puerto;
    int tamanoCache;
    int socketServidor;
    string motorHost;
    int motorPort;
    
    // Cache LRU: hashmap + list para orden de uso
    unordered_map<string, pair<string, list<string>::iterator>> cache;
    list<string> ordenUso; // Claves ordenadas por uso (LRU al final)
    mutex cacheMutex;
    
    bool inicializarSocket();
    void manejarCliente(int socketCliente);
    string consultarMotor(const string& palabra, const string& archivoIndice);
    void agregarACache(const string& clave, const string& valor);
    string obtenerDeCache(const string& clave);
    
public:
    CacheServer(int puerto, int tamanoCache, const string& motorHost, int motorPort);
    ~CacheServer();
    void iniciar();
    void detener();
};

#endif
