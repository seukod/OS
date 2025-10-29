#ifndef OS_CREAR_INDICE_INV_H
#define OS_CREAR_INDICE_INV_H

#include <filesystem>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <chrono>
#include <limits>

// -----------------------------
// Estructuras originales
// -----------------------------
struct ConfigParalelo {
    int nThreads;
    int nLote;
    ConfigParalelo(int threads = 4, int lote = 10) : nThreads(threads), nLote(lote) {}
};

struct DocumentoInfo {
    int libroId;
    int cantidad;
    DocumentoInfo(int id, int cant) : libroId(id), cantidad(cant) {}
};

// -----------------------------
// Funciones originales
// -----------------------------
std::vector<std::string> escanearLibros(const std::string& rutaLibros);
void actualizarMapaLibros(const std::vector<std::string>& libros);
void crearIndiceInvertidoParalelo(const std::string& archivoSalida, const std::string& directorioLibros, const ConfigParalelo& config);
void procesarLoteLibros(const std::vector<std::string>& loteLibros,
                        const std::string& directorioLibros,
                        std::unordered_map<std::string, std::vector<DocumentoInfo>>& indiceGlobal,
                        std::mutex& mutexIndice,
                        const std::unordered_map<std::string, int>& mapaLibros);
int solicitarCantidadThreads();
int solicitarCantidadLotes();
std::string limpiarPalabra(const std::string& palabra);

// -----------------------------
// agregado para logging global
// -----------------------------
extern std::mutex mutexLog;
extern std::ofstream logFile;

void registrarLog(const std::thread::id& tid, int libroId, int totalPalabras,
                  const std::chrono::system_clock::time_point& inicio,
                  const std::chrono::system_clock::time_point& fin);

#endif // OS_CREAR_INDICE_INV_H
