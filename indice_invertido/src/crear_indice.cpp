#include "../include/crear_indice.h"
#include <fstream>
#include <algorithm>
#include <dirent.h>
#include <sys/stat.h>
#include <cctype>



string limpiarPalabra(const string& palabra) {
    // La palabra ya viene limpia del procesamiento línea por línea,
    // solo aplicamos filtros para evitar palabras mal formadas
    
    // No palabras vacías
    if (palabra.empty()) {
        return "";
    }
    
    return palabra;
}

bool procesarDirectorio(const string& directorioLibros, unordered_map<string, vector<DocumentoInfo>>& indiceInvertido) {
    DIR* dir = opendir(directorioLibros.c_str());
    if (!dir) {
        cout << "ERROR: No se pudo abrir el directorio: " << directorioLibros << endl;
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
                procesarArchivo(nombreArchivo, rutaCompleta, indiceInvertido);
                archivosProcesados++;
            }
        }
    }
    
    closedir(dir);
    
    cout << "Total de archivos procesados: " << archivosProcesados << endl;
    return archivosProcesados > 0;
}

void procesarArchivo(const string& nombreArchivo, const string& rutaCompleta, unordered_map<string, vector<DocumentoInfo>>& indiceInvertido) {
    ifstream archivo(rutaCompleta);
    if (!archivo.is_open()) {
        cout << "No se pudo abrir el archivo: " << nombreArchivo << endl;
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

bool guardarIndice(const string& archivoSalida, const unordered_map<string, vector<DocumentoInfo>>& indiceInvertido) {
    ofstream archivo(archivoSalida);
    if (!archivo.is_open()) {
        cout << "ERROR: No se pudo crear el archivo: " << archivoSalida << endl;
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
        
        const vector<DocumentoInfo>& documentos = indiceInvertido.at(palabra);
        for (const DocumentoInfo& doc : documentos) {
            archivo << ";(" << doc.nombre << "," << doc.cantidad << ")";
        }
        
        archivo << endl;
    }
    
    archivo.close();
    cout << "Índice guardado exitosamente." << endl;
    return true;
}

bool crearIndice(const string& archivoSalida, const string& directorioLibros) {
    cout << "=================================================" << endl;
    cout << "         CREADOR DE ÍNDICE INVERTIDO             " << endl;
    cout << "=================================================" << endl;
    cout << "Directorio de entrada: " << directorioLibros << endl;
    cout << "Archivo de salida: " << archivoSalida << endl;
    cout << "Proceso indice invertido iniciado, PID: " << getpid() << endl;
    cout << "=================================================" << endl;
    
    // HashMap que almacena: palabra -> vector de documentos donde aparece
    unordered_map<string, vector<DocumentoInfo>> indiceInvertido;
    
    // Procesar todos los archivos
    if (!procesarDirectorio(directorioLibros, indiceInvertido)) {
        cout << "ERROR: No se pudieron procesar los archivos." << endl;
        return false;
    }
    
    // Guardar el índice
    if (!guardarIndice(archivoSalida, indiceInvertido)) {
        cout << "ERROR: No se pudo guardar el índice." << endl;
        return false;
    }
    
    return true;
}