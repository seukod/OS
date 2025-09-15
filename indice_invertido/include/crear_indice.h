#ifndef CREAR_INDICE_H
#define CREAR_INDICE_H

#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include "../../include/process_tools/process_manager.h"

using namespace std;

// Estructura para almacenar la información de cada documento
struct DocumentoInfo {
    string nombre;
    int cantidad;
    
    DocumentoInfo(const string& nom, int cant) : nombre(nom), cantidad(cant) {}
};

// Declaraciones de funciones

// Procesa todos los archivos en el directorio
bool procesarDirectorio(const string& directorioLibros, unordered_map<string, vector<DocumentoInfo>>& indiceInvertido);

// Procesa un archivo individual
void procesarArchivo(const string& nombreArchivo, const string& rutaCompleta, unordered_map<string, vector<DocumentoInfo>>& indiceInvertido);

// Limpia y normaliza una palabra (quita signos de puntuación, convierte a minúscula)
string limpiarPalabra(const string& palabra);

// Guarda el índice invertido en el archivo de salida
bool guardarIndice(const string& archivoSalida, const unordered_map<string, vector<DocumentoInfo>>& indiceInvertido);

// Función principal que ejecuta todo el proceso
bool crearIndice(const string& archivoSalida, const string& directorioLibros);

// Muestra estadísticas del proceso
void mostrarEstadisticas(const unordered_map<string, vector<DocumentoInfo>>& indiceInvertido);

#endif // CREAR_INDICE_H
