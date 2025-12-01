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
    
    // Verificar si se pasó el archivo .idx como argumento
    string archivoIdx = "";
    if (argc > 1) {
        archivoIdx = argv[1];
        cout << "[CONFIG] Archivo índice: " << archivoIdx << endl;
    }
    
    // Crear instancia del buscador
    Buscador buscador(host, port);
    
    // Si se pasó archivo, buscar directamente; sino, mostrar menú
    if (!archivoIdx.empty()) {
        // Solicitar solo la palabra a buscar
        string palabra;
        cout << "\nIngrese la palabra a buscar: ";
        getline(cin, palabra);
        
        if (!palabra.empty()) {
            buscador.buscar(palabra);
        }
    } else {
        // Iniciar interfaz completa (seleccionar archivo + buscar)
        buscador.iniciar();
    }
    
    return 0;
}
