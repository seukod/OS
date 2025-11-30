#include <iostream>
#include <unistd.h>

using namespace std;

int main() {
    cout << "[CACHE] Servidor iniciado" << endl;
    cout << "[CACHE] PID: " << getpid() << endl;
    cout << "[TODO] Implementar servidor de cache con sockets" << endl;
    
    // TODO: Implementar servidor de cache
    // - Escuchar en puerto definido por CACHE_PORT
    // - Mantener diccionario con CACHE_SIZE elementos
    // - Conectar con MOTOR_BUSQUEDA cuando no haya hit en cache
    // - Devolver respuestas en formato JSON
    
    return 0;
}
