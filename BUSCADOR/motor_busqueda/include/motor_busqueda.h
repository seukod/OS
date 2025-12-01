#ifndef MOTOR_BUSQUEDA_H
#define MOTOR_BUSQUEDA_H

#include <string>
#include <vector>
#include <unordered_map>
#include <map>

using namespace std;

struct InfoLibro {
    int frecuencia;
    vector<int> posiciones;
};

struct ResultadoScore {
    string nombreLibro;
    double score;
    
    bool operator>(const ResultadoScore& otro) const {
        return score > otro.score;
    }
};

class MotorBusqueda {
private:
    int puerto;
    int topK;
    int socketServidor;
    
    // Índice invertido: palabra -> {idLibro -> InfoLibro}
    unordered_map<string, unordered_map<int, InfoLibro>> indiceInvertido;
    
    // Mapeo ID -> Nombre del libro
    unordered_map<int, string> mapaLibros;
    
    bool inicializarSocket();
    void manejarCliente(int socketCliente);
    bool cargarIndiceInvertido(const string& rutaIdx);
    bool cargarMapaLibros(const string& rutaMapa);
    vector<string> separarPalabras(const string& consulta);
    string buscar(const vector<string>& palabras);
    vector<ResultadoScore> calcularScores(const vector<string>& palabras);
    string generarJSON(const vector<ResultadoScore>& resultados);
    
public:
    MotorBusqueda(int puerto, int topK);
    ~MotorBusqueda();
    void cargarRecursos(const string& rutaIdx, const string& rutaMapa);
    void iniciar();
    void detener();
};

#endif
