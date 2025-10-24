#include "../include/Crear_indice_inv.h"

using namespace std;

vector<string> escanearLibros(const string& rutaLibros) {
    vector<string> nombresLibros;

    for (const auto& entrada: filesystem::directory_iterator(rutaLibros)) {
        if (entrada.is_regular_file()) {
            string nombreArchivo = entrada.path().filename().string();

            // Cambiar ends_with() por substr() para compatibilidad con C++17
            if (nombreArchivo.length() >= 4 &&
                nombreArchivo.substr(nombreArchivo.length() - 4) == ".txt") {
                nombresLibros.push_back(nombreArchivo);
                cout << "Libro encontrado: " << nombreArchivo << endl;
            }
        }
    }
    return nombresLibros;
}

void actualizarMapaLibros(const vector<string>& libros) {
    string rutaMapa = "../../data/MAPA-LIBROS.csv";
    ofstream archivoMapa(rutaMapa);

    if (!archivoMapa.is_open()) {
        cout << "ERROR: No se pudo crear el archivo de mapa de libros." << endl;
        return;
    }

    // Escribir encabezado
    archivoMapa << "ID,NOMBRE_LIBRO" << endl;

    // Corregir el tipo del bucle para evitar warning
    for (size_t i = 0; i < libros.size(); ++i) {
        archivoMapa << (i + 1) << "," << libros[i] << endl;
    }

    archivoMapa.close();
    cout << "Mapa de libros creado exitosamente: " << rutaMapa << endl;
    cout << "Total de libros registrados: " << libros.size() << endl;
}

/*
void crearIndiceInvertidoParalelo() {

}
*/

// Función para limpiar y normalizar palabras
string limpiarPalabra(const string& palabra) {
    if (palabra.empty()) return "";

    string palabraLimpia;
    for (char c : palabra) {
        if (isalpha(c)) {
            palabraLimpia += tolower(c);
        }
    }
    return palabraLimpia.empty() ? "" : palabraLimpia;
}

// Función para cargar el mapa de libros desde el CSV
unordered_map<string, int> cargarMapaLibros(const string& rutaMapa) {
    unordered_map<string, int> mapa;
    ifstream archivo(rutaMapa);

    if (!archivo.is_open()) {
        cout << "ERROR: No se pudo cargar el mapa de libros desde " << rutaMapa << endl;
        return mapa;
    }

    string linea;
    getline(archivo, linea); // Saltar encabezado

    while (getline(archivo, linea)) {
        size_t coma = linea.find(',');
        if (coma != string::npos) {
            int id = stoi(linea.substr(0, coma));
            string nombre = linea.substr(coma + 1);
            mapa[nombre] = id;
        }
    }

    archivo.close();
    cout << "Mapa de libros cargado: " << mapa.size() << " entradas" << endl;
    return mapa;
}

// Función para procesar un lote de libros en paralelo
void procesarLoteLibros(const vector<string>& loteLibros, const string& directorioLibros,
                       unordered_map<string, vector<DocumentoInfo>>& indiceGlobal,
                       mutex& mutexIndice, const unordered_map<string, int>& mapaLibros) {

    // Índice local para este thread
    unordered_map<string, vector<DocumentoInfo>> indiceLocal;

    cout << "[THREAD " << this_thread::get_id() << "] Procesando lote de " << loteLibros.size() << " libros" << endl;

    for (const string& nombreLibro : loteLibros) {
        string rutaCompleta = directorioLibros + "/" + nombreLibro;
        ifstream archivo(rutaCompleta);

        if (!archivo.is_open()) {
            cout << "[ERROR] No se pudo abrir: " << nombreLibro << endl;
            continue;
        }

        // Obtener ID del libro
        auto it = mapaLibros.find(nombreLibro);
        if (it == mapaLibros.end()) {
            cout << "[ERROR] ID no encontrado para: " << nombreLibro << endl;
            continue;
        }
        int libroId = it->second;

        // Procesar contenido del archivo
        unordered_map<string, int> contadorPalabras;
        string linea;

        while (getline(archivo, linea)) {
            string palabraActual = "";

            for (char c : linea) {
                if (isalpha(c)) {
                    palabraActual += tolower(c);
                } else {
                    if (!palabraActual.empty()) {
                        string palabraLimpia = limpiarPalabra(palabraActual);
                        if (!palabraLimpia.empty()) {
                            contadorPalabras[palabraLimpia]++;
                        }
                        palabraActual = "";
                    }
                }
            }

            // Procesar última palabra de la línea
            if (!palabraActual.empty()) {
                string palabraLimpia = limpiarPalabra(palabraActual);
                if (!palabraLimpia.empty()) {
                    contadorPalabras[palabraLimpia]++;
                }
            }
        }

        archivo.close();

        // Agregar al índice local
        for (const auto& par : contadorPalabras) {
            indiceLocal[par.first].emplace_back(libroId, par.second);
        }

        cout << "[THREAD " << this_thread::get_id() << "] Procesado: " << nombreLibro << " (ID: " << libroId << ")" << endl;
    }

    // Fusionar índice local con el global (sección crítica)
    {
        lock_guard<mutex> lock(mutexIndice);
        for (const auto& par : indiceLocal) {
            const string& palabra = par.first;
            const vector<DocumentoInfo>& documentos = par.second;

            for (const DocumentoInfo& doc : documentos) {
                indiceGlobal[palabra].push_back(doc);
            }
        }
    }

    cout << "[THREAD " << this_thread::get_id() << "] Lote completado y fusionado al índice global" << endl;
}

// Función principal para crear el índice invertido paralelo
void crearIndiceInvertidoParalelo(const string& archivoSalida, const string& directorioLibros, const ConfigParalelo& config) {
    cout << "\n=================================================" << endl;
    cout << "      ÍNDICE INVERTIDO PARALELO - INICIANDO     " << endl;
    cout << "=================================================" << endl;
    cout << "N-THREADS: " << config.nThreads << endl;
    cout << "N-LOTE: " << config.nLote << endl;
    cout << "Directorio: " << directorioLibros << endl;
    cout << "Archivo salida: " << archivoSalida << endl;
    cout << "=================================================" << endl;

    // 1. Escanear libros
    vector<string> libros = escanearLibros(directorioLibros);
    if (libros.empty()) {
        cout << "ERROR: No se encontraron libros" << endl;
        return;
    }

    // 2. Crear mapa de libros
    actualizarMapaLibros(libros);
    unordered_map<string, int> mapaLibros = cargarMapaLibros("../../data/MAPA-LIBROS.csv");

    // 3. Dividir libros en lotes
    vector<vector<string>> lotes;
    for (size_t i = 0; i < libros.size(); i += config.nLote) {
        vector<string> lote;
        for (size_t j = i; j < min(i + config.nLote, libros.size()); ++j) {
            lote.push_back(libros[j]);
        }
        lotes.push_back(lote);
    }

    cout << "Total de libros: " << libros.size() << endl;
    cout << "Total de lotes: " << lotes.size() << endl;

    // 4. Procesar lotes en paralelo
    unordered_map<string, vector<DocumentoInfo>> indiceGlobal;
    mutex mutexIndice;
    vector<thread> hilos;

    // Crear y lanzar threads
    for (size_t i = 0; i < lotes.size(); ++i) {
        if (hilos.size() >= config.nThreads) {
            // Esperar a que termine un thread antes de crear uno nuevo
            hilos[i % config.nThreads].join();
            hilos[i % config.nThreads] = thread(procesarLoteLibros, lotes[i], directorioLibros,
                                               ref(indiceGlobal), ref(mutexIndice), ref(mapaLibros));
        } else {
            hilos.emplace_back(procesarLoteLibros, lotes[i], directorioLibros,
                              ref(indiceGlobal), ref(mutexIndice), ref(mapaLibros));
        }
    }

    // Esperar a que terminen todos los threads
    for (auto& hilo : hilos) {
        if (hilo.joinable()) {
            hilo.join();
        }
    }

    // 5. Guardar índice
    cout << "\nGuardando índice invertido..." << endl;
    ofstream archivo(archivoSalida);
    if (!archivo.is_open()) {
        cout << "ERROR: No se pudo crear " << archivoSalida << endl;
        return;
    }

    for (const auto& par : indiceGlobal) {
        archivo << par.first;
        for (const DocumentoInfo& doc : par.second) {
            archivo << ";(" << doc.libroId << "," << doc.cantidad << ")";
        }
        archivo << endl;
    }

    archivo.close();

    cout << "=================================================" << endl;
    cout << "ÍNDICE INVERTIDO PARALELO COMPLETADO" << endl;
    cout << "Palabras procesadas: " << indiceGlobal.size() << endl;
    cout << "Archivo generado: " << archivoSalida << endl;
    cout << "=================================================" << endl;
}
