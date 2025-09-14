#ifndef CREAR_INDICE_H
#define CREAR_INDICE_H

#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>

using namespace std;

// Estructura para almacenar la información de cada documento
struct DocumentoInfo {
    string nombre;
    int cantidad;
    
    DocumentoInfo(const string& nom, int cant) : nombre(nom), cantidad(cant) {}
};

class CreadorIndice {
private:
    // HashMap que almacena: palabra -> vector de documentos donde aparece
    unordered_map<string, vector<DocumentoInfo>> indiceInvertido;
    string archivoSalida;
    string directorioLibros;
    
public:
    CreadorIndice(const string& archivo, const string& directorio);
    
    // Procesa todos los archivos en el directorio
    bool procesarDirectorio();
    
    // Procesa un archivo individual
    void procesarArchivo(const string& nombreArchivo, const string& rutaCompleta);
    
    // Limpia y normaliza una palabra (quita signos de puntuación, convierte a minúscula)
    string limpiarPalabra(const string& palabra);
    
    // Guarda el índice invertido en el archivo de salida
    bool guardarIndice();
    
    // Función principal que ejecuta todo el proceso
    bool crearIndice();
    
    // Muestra estadísticas del proceso
    void mostrarEstadisticas();
};

#endif // CREAR_INDICE_H
