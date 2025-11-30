#ifndef BUSCADOR_H
#define BUSCADOR_H

#include <string>
#include <vector>
#include <map>

// Estructura para almacenar resultados de búsqueda
struct ResultadoBusqueda {
    std::string libro;
    int frecuencia;
    std::vector<int> posiciones;
};

// Clase para manejar la comunicación con el servidor CACHE
class Buscador {
private:
    int socketFd;
    std::string cacheHost;
    int cachePort;
    std::map<int, std::string> mapaLibros; // ID -> nombre del libro
    
    bool conectarCache();
    void cerrarConexion();
    std::string enviarConsulta(const std::string& palabra);
    void cargarMapaLibros(const std::string& rutaMapa);
    std::vector<ResultadoBusqueda> parsearRespuestaJSON(const std::string& json);
    void mostrarResultados(const std::vector<ResultadoBusqueda>& resultados, const std::string& palabra);
    
public:
    Buscador(const std::string& host, int port);
    ~Buscador();
    
    void iniciar();
    bool validarIndiceInvertido(const std::string& rutaIndice);
    void buscar(const std::string& palabra);
};

#endif // BUSCADOR_H
