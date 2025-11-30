#include "../include/buscador.h"
#include <iostream>
#include <cstdlib>

using namespace std;

int main(int argc, char* argv[]) {
    // Obtener configuración desde variables de entorno o argumentos
    const char* cacheHost = getenv("CACHE_HOST");
    const char* cachePortStr = getenv("CACHE_PORT");
    
    // Valores por defecto
    string host = cacheHost ? cacheHost : "127.0.0.1";
    int port = cachePortStr ? atoi(cachePortStr) : 8080;
    
    cout << "[BUSCADOR SistOpe] Iniciando..." << endl;
    cout << "[CONFIG] CACHE_HOST: " << host << endl;
    cout << "[CONFIG] CACHE_PORT: " << port << endl;
    
    // Crear instancia del buscador
    Buscador buscador(host, port);
    
    // Iniciar interfaz de búsqueda (el usuario seleccionará el archivo .idx)
    buscador.iniciar();
    
    return 0;
}
