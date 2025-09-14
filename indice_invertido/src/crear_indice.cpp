#include "../include/crear_indice.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <dirent.h>
#include <sys/stat.h>
#include <cctype>

CreadorIndice::CreadorIndice(const string& archivo, const string& directorio) 
    : archivoSalida(archivo), directorioLibros(directorio) {}

string CreadorIndice::limpiarPalabra(const string& palabra) {
    // La palabra ya viene limpia del procesamiento línea por línea,
    // solo aplicamos filtros para evitar palabras mal formadas
    
    // 1. No palabras vacías
    if (palabra.empty()) {
        return "";
    }
    
    // 2. No palabras con solo un carácter repetido (como "aaa", "bbb")
    if (palabra.length() > 2) {
        bool todasIguales = true;
        char primerChar = palabra[0];
        for (char c : palabra) {
            if (c != primerChar) {
                todasIguales = false;
                break;
            }
        }
        if (todasIguales) {
            return "";
        }
    }
    
    // 3. No palabras con patrones raros (más de 3 caracteres consecutivos iguales)
    // Verificamos que tengamos al menos 3 caracteres antes de acceder
    if (palabra.length() >= 3) {
        for (size_t i = 0; i <= palabra.length() - 3; i++) {
            if (palabra[i] == palabra[i+1] && palabra[i+1] == palabra[i+2]) {
                return "";
            }
        }
    }
    
    return palabra;
}

bool CreadorIndice::procesarDirectorio() {
    DIR* dir = opendir(directorioLibros.c_str());
    if (!dir) {
        cout << "[ERROR] No se pudo abrir el directorio: " << directorioLibros << endl;
        return false;
    }
    
    cout << "Procesando archivos en: " << directorioLibros << endl;
    
    struct dirent* entrada;
    int archivosProcesados = 0;
    
    while ((entrada = readdir(dir)) != nullptr) {
        string nombreArchivo = entrada->d_name;
        
        // Ignorar . y ..
        if (nombreArchivo == "." || nombreArchivo == "..") {
            continue;
        }
        
        string rutaCompleta = directorioLibros + "/" + nombreArchivo;
        
        // Verificar si es un archivo regular
        struct stat fileStat;
        if (stat(rutaCompleta.c_str(), &fileStat) == 0 && S_ISREG(fileStat.st_mode)) {
            // Solo procesar archivos .txt
            if (nombreArchivo.length() > 4 && 
                nombreArchivo.substr(nombreArchivo.length() - 4) == ".txt") {
                
                cout << "Procesando: " << nombreArchivo << endl;
                procesarArchivo(nombreArchivo, rutaCompleta);
                archivosProcesados++;
            }
        }
    }
    
    closedir(dir);
    
    cout << "Total de archivos procesados: " << archivosProcesados << endl;
    return archivosProcesados > 0;
}

void CreadorIndice::procesarArchivo(const string& nombreArchivo, const string& rutaCompleta) {
    ifstream archivo(rutaCompleta);
    if (!archivo.is_open()) {
        cout << "[WARNING] No se pudo abrir el archivo: " << nombreArchivo << endl;
        return;
    }
    
    // HashMap para contar palabras en este documento específico
    unordered_map<string, int> contadorPalabras;
    
    string linea;
    while (getline(archivo, linea)) {
        // Procesar cada línea completa desde \n hasta \n
        string palabraActual = "";
        
        for (char c : linea) {
            if (isalpha(c)) {
                // Si es una letra, agregar a la palabra actual
                palabraActual += tolower(c);
            } else {
                // Si no es letra, significa que terminó una palabra
                if (!palabraActual.empty()) {
                    string palabraLimpia = limpiarPalabra(palabraActual);
                    if (!palabraLimpia.empty()) {
                        contadorPalabras[palabraLimpia]++;
                    }
                    palabraActual = ""; // Resetear para la siguiente palabra
                }
            }
        }
        
        // Procesar la última palabra de la línea si existe
        if (!palabraActual.empty()) {
            string palabraLimpia = limpiarPalabra(palabraActual);
            if (!palabraLimpia.empty()) {
                contadorPalabras[palabraLimpia]++;
            }
        }
    }
    
    archivo.close();
    
    // Agregar las palabras encontradas al índice invertido global
    for (const auto& par : contadorPalabras) {
        const string& palabra = par.first;
        int cantidad = par.second;
        
        // Buscar si ya existe la palabra en el índice
        auto& documentos = indiceInvertido[palabra];
        
        // Agregar este documento a la lista
        documentos.emplace_back(nombreArchivo, cantidad);
    }
}

bool CreadorIndice::guardarIndice() {
    ofstream archivo(archivoSalida);
    if (!archivo.is_open()) {
        cout << "[ERROR] No se pudo crear el archivo: " << archivoSalida << endl;
        return false;
    }
    
    cout << "\nGuardando índice invertido en: " << archivoSalida << endl;
    
    // Ordenar palabras alfabéticamente para un output más legible
    vector<string> palabrasOrdenadas;
    for (const auto& par : indiceInvertido) {
        palabrasOrdenadas.push_back(par.first);
    }
    sort(palabrasOrdenadas.begin(), palabrasOrdenadas.end());
    
    // Escribir cada palabra con sus documentos
    for (const string& palabra : palabrasOrdenadas) {
        archivo << palabra;
        
        const vector<DocumentoInfo>& documentos = indiceInvertido[palabra];
        for (const DocumentoInfo& doc : documentos) {
            archivo << ";(" << doc.nombre << "," << doc.cantidad << ")";
        }
        
        archivo << endl;
    }
    
    archivo.close();
    cout << "Índice guardado exitosamente." << endl;
    return true;
}

bool CreadorIndice::crearIndice() {
    cout << "=================================================" << endl;
    cout << "         CREADOR DE ÍNDICE INVERTIDO             " << endl;
    cout << "=================================================" << endl;
    cout << "Directorio de entrada: " << directorioLibros << endl;
    cout << "Archivo de salida: " << archivoSalida << endl;
    cout << "=================================================" << endl;
    
    // Paso 1: Procesar todos los archivos
    if (!procesarDirectorio()) {
        cout << "[ERROR] No se pudieron procesar los archivos." << endl;
        return false;
    }
    
    // Paso 2: Guardar el índice
    if (!guardarIndice()) {
        cout << "[ERROR] No se pudo guardar el índice." << endl;
        return false;
    }
    
    return true;
}
