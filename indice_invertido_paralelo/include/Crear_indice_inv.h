//
// Created by Francisco Contreras on 24-10-25.
//

#ifndef OS_CREAR_INDICE_INV_H
#define OS_CREAR_INDICE_INV_H

#include <filesystem>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unistd.h>  // Para getpid()
#include <thread>    // Para std::thread
#include <mutex>     // Para std::mutex
#include <unordered_map>
#include <queue>

// Estructura para configuración del procesamiento paralelo
struct ConfigParalelo {
    int nThreads;
    int nLote;

    ConfigParalelo(int threads = 4, int lote = 10) : nThreads(threads), nLote(lote) {}
};

// Estructura para información de documentos en el índice
struct DocumentoInfo {
    int libroId;
    int cantidad;

    DocumentoInfo(int id, int cant) : libroId(id), cantidad(cant) {}
};

// Funciones existentes
std::vector<std::string> escanearLibros(const std::string& rutaLibros);
void actualizarMapaLibros(const std::vector<std::string>& libros);

// Nuevas funciones para procesamiento paralelo
void crearIndiceInvertidoParalelo(const std::string& archivoSalida, const std::string& directorioLibros, const ConfigParalelo& config);
void procesarLoteLibros(const std::vector<std::string>& loteLibros, const std::string& directorioLibros,
                       std::unordered_map<std::string, std::vector<DocumentoInfo>>& indiceGlobal,
                       std::mutex& mutexIndice, const std::unordered_map<std::string, int>& mapaLibros);


#endif //OS_CREAR_INDICE_INV_H
