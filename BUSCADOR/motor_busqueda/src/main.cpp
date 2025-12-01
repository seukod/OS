#include "../include/motor_busqueda.h"
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
    cout << "=================================================" << endl;
    cout << "       SERVIDOR MOTOR_BUSQUEDA - SISTOPE         " << endl;
    cout << "=================================================" << endl;
    cout << "[MOTOR] PID: " << getpid() << endl;
    
    // Leer configuración
    const char* topkStr = getenv("TOPK");
    int topK = topkStr ? atoi(topkStr) : 10;
    int puerto = 8081; // Puerto fijo para motor de búsqueda
    
    cout << "[CONFIG] Puerto: " << puerto << endl;
    cout << "[CONFIG] TOP-K: " << topK << endl;
    
    // Verificar argumentos: ./motor_busqueda <archivo.idx>
    if (argc < 2) {
        cerr << "[ERROR] Uso: " << argv[0] << " <archivo.idx>" << endl;
        cerr << "[ERROR] Ejemplo: " << argv[0] << " ../nose.idx" << endl;
        return 1;
    }
    
    string rutaIdx = argv[1];
    string rutaMapa = "../data/MAPA-LIBROS.csv";
    
    cout << "[CONFIG] Índice: " << rutaIdx << endl;
    cout << "[CONFIG] Mapa: " << rutaMapa << endl;
    cout << "=================================================" << endl;
    
    // Crear motor de búsqueda
    MotorBusqueda motor(puerto, topK);
    
    // Cargar recursos
    motor.cargarRecursos(rutaIdx, rutaMapa);
    
    // Iniciar servidor
    motor.iniciar();
    
    return 0;
}
