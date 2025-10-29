#include "../include/Crear_indice_inv.h"

using namespace std;

// ============================================================
// agregado para logging global
// ============================================================
mutex mutexLog;
ofstream logFile("../data/log_procesamiento.txt");

void registrarLog(const thread::id& tid, int libroId, int totalPalabras,
                  const chrono::system_clock::time_point& inicio,
                  const chrono::system_clock::time_point& fin) {
    lock_guard<mutex> lock(mutexLog);
    logFile << "THREAD: " << tid
            << ", LIBRO: " << libroId
            << ", PALABRAS: " << totalPalabras
            << ", INICIO: " << chrono::duration_cast<chrono::milliseconds>(inicio.time_since_epoch()).count()
            << ", FIN: " << chrono::duration_cast<chrono::milliseconds>(fin.time_since_epoch()).count()
            << endl;
}

// ============================================================
// funciones originales (no se modifican, solo se usan)
// ============================================================

int solicitarCantidadLotes() {
    int nLote;
    while (true) {
        cout << "Ingrese la cantidad de libros por lote (mayor a 0): ";
        if (!(cin >> nLote)) {
            cout << "Entrada inválida. Ingrese un número entero." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        if (nLote <= 0) cout << "El tamaño de lote debe ser mayor que 0." << endl;
        else break;
    }
    return nLote;
}

int solicitarCantidadThreads() {
    unsigned int maxThreads = thread::hardware_concurrency();
    if (maxThreads == 0) maxThreads = 4;
    cout << "Número máximo de hilos disponibles: " << maxThreads << endl;

    int nThreads;
    while (true) {
        cout << "Ingrese la cantidad de hilos a utilizar (1 - " << maxThreads << "): ";
        if (!(cin >> nThreads)) {
            cout << "Entrada inválida. Ingrese un número entero." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        if (nThreads < 1 || nThreads > (int)maxThreads) cout << "Cantidad fuera de rango." << endl;
        else break;
    }
    return nThreads;
}

vector<string> escanearLibros(const string& rutaLibros) {
    vector<string> nombresLibros;
    for (const auto& entrada: filesystem::directory_iterator(rutaLibros)) {
        if (entrada.is_regular_file()) {
            string nombreArchivo = entrada.path().filename().string();
            if (nombreArchivo.length() >= 4 && nombreArchivo.substr(nombreArchivo.length() - 4) == ".txt") {
                nombresLibros.push_back(nombreArchivo);
                cout << "Libro encontrado: " << nombreArchivo << endl;
            }
        }
    }
    return nombresLibros;
}

void actualizarMapaLibros(const vector<string>& libros) {
    string rutaMapa = "../data/MAPA-LIBROS.csv";
    ofstream archivoMapa(rutaMapa);
    if (!archivoMapa.is_open()) {
        cout << "ERROR: No se pudo crear archivo de mapa de libros" << endl;
        return;
    }
    archivoMapa << "ID,NOMBRE_LIBRO" << endl;
    for (size_t i = 0; i < libros.size(); ++i)
        archivoMapa << (i + 1) << "," << libros[i] << endl;

    archivoMapa.close();
    cout << "Mapa de libros creado exitosamente: " << rutaMapa << endl;
    cout << "Total de libros registrados: " << libros.size() << endl;
}

string limpiarPalabra(const string& palabra) {
    string palabraLimpia;
    for (char c : palabra)
        if (isalpha(c)) palabraLimpia += tolower(c);
    return palabraLimpia;
}

// ============================================================
// funciones nuevas y paralelas (usa log global agregado)
// ============================================================

unordered_map<string, int> cargarMapaLibros(const string& rutaMapa) {
    unordered_map<string, int> mapa;
    ifstream archivo(rutaMapa);
    if (!archivo.is_open()) {
        cout << "ERROR: No se pudo cargar el mapa de libros" << endl;
        return mapa;
    }
    string linea;
    getline(archivo, linea);
    while (getline(archivo, linea)) {
        size_t coma = linea.find(',');
        if (coma != string::npos) {
            int id = stoi(linea.substr(0, coma));
            string nombre = linea.substr(coma + 1);
            mapa[nombre] = id;
        }
    }
    archivo.close();
    return mapa;
}

void procesarLoteLibros(const vector<string>& loteLibros, const string& directorioLibros,
                        unordered_map<string, vector<DocumentoInfo>>& indiceGlobal,
                        mutex& mutexIndice,
                        const unordered_map<string, int>& mapaLibros) {

    unordered_map<string, vector<DocumentoInfo>> indiceLocal;

    for (const string& nombreLibro : loteLibros) {
        auto inicio = chrono::system_clock::now();

        auto it = mapaLibros.find(nombreLibro);
        if (it == mapaLibros.end()) continue;
        int libroId = it->second;

        ifstream archivo(directorioLibros + "/" + nombreLibro);
        if (!archivo.is_open()) continue;

        unordered_map<string, int> contadorPalabras;
        string linea;
        while (getline(archivo, linea)) {
            string palabraActual;
            for (char c : linea) {
                if (isalpha(c)) palabraActual += tolower(c);
                else if (!palabraActual.empty()) {
                    contadorPalabras[limpiarPalabra(palabraActual)]++;
                    palabraActual.clear();
                }
            }
            if (!palabraActual.empty()) contadorPalabras[limpiarPalabra(palabraActual)]++;
        }
        archivo.close();

        for (const auto& par : contadorPalabras)
            indiceLocal[par.first].emplace_back(libroId, par.second);

        auto fin = chrono::system_clock::now();
        int totalPalabras = 0;
        for (auto& p : contadorPalabras)
            totalPalabras += p.second;

        // 🔹 registrar log sin pasarlo como parámetro
        registrarLog(this_thread::get_id(), libroId, totalPalabras, inicio, fin);
    }

    lock_guard<mutex> lock(mutexIndice);
    for (const auto& par : indiceLocal)
        for (const auto& doc : par.second)
            indiceGlobal[par.first].push_back(doc);
}

void crearIndiceInvertidoParalelo(const string& archivoSalida, const string& directorioLibros, const ConfigParalelo& config) {
    cout << "\n=== INICIO ÍNDICE INVERTIDO PARALELO ===" << endl;
    cout << "Threads: " << config.nThreads << ", Lote: " << config.nLote << endl;

    vector<string> libros = escanearLibros(directorioLibros);
    if (libros.empty()) {
        cout << "No se encontraron libros" << endl;
        return;
    }

    actualizarMapaLibros(libros);
    unordered_map<string, int> mapaLibros = cargarMapaLibros("../data/MAPA-LIBROS.csv");

    // dividir en lotes
    vector<vector<string>> lotes;
    for (size_t i = 0; i < libros.size(); i += config.nLote) {
        vector<string> lote;
        for (size_t j = i; j < min(i + config.nLote, libros.size()); ++j)
            lote.push_back(libros[j]);
        lotes.push_back(lote);
    }

    unordered_map<string, vector<DocumentoInfo>> indiceGlobal;
    mutex mutexIndice;
    vector<thread> hilos;

    for (size_t i = 0; i < lotes.size(); ++i) {
        if (hilos.size() >= static_cast<size_t>(config.nThreads)) {
            hilos[i % config.nThreads].join();
            hilos[i % config.nThreads] = thread(procesarLoteLibros, lotes[i], directorioLibros,
                                                ref(indiceGlobal), ref(mutexIndice), ref(mapaLibros));
        } else {
            hilos.emplace_back(procesarLoteLibros, lotes[i], directorioLibros,
                               ref(indiceGlobal), ref(mutexIndice), ref(mapaLibros));
        }
    }

    for (auto& hilo : hilos)
        if (hilo.joinable()) hilo.join();

    ofstream archivo(archivoSalida);
    if (!archivo.is_open()) {
        cout << "ERROR: No se pudo crear " << archivoSalida << endl;
        return;
    }
    for (const auto& par : indiceGlobal) {
        archivo << par.first;
        for (const auto& doc : par.second)
            archivo << ";(" << doc.libroId << "," << doc.cantidad << ")";
        archivo << endl;
    }
    archivo.close();

    cout << "=== ÍNDICE INVERTIDO COMPLETADO ===" << endl;
}
