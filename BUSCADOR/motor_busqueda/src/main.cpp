#include <iostream>
#include <unistd.h>

using namespace std;

int main() {
    cout << "[MOTOR_BUSQUEDA] Servidor iniciado" << endl;
    cout << "[MOTOR_BUSQUEDA] PID: " << getpid() << endl;
    cout << "[TODO] Implementar motor de búsqueda con sockets" << endl;
    
    // TODO: Implementar motor de búsqueda
    // - Escuchar en puerto definido por MOTOR_PORT
    // - Cargar índice invertido en memoria
    // - Implementar algoritmo TOP-K (variable TOPK)
    // - Devolver respuestas en formato JSON con IDs de libros
    
    return 0;
}
