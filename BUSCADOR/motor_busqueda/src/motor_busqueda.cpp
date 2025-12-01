#include "../include/motor_busqueda.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <queue>
#include <set>

using namespace std;

MotorBusqueda::MotorBusqueda(int puerto, int topK)
    : puerto(puerto), topK(topK), socketServidor(-1) {
    cout << "[MOTOR] Inicializado" << endl;
    cout << "[MOTOR] Puerto: " << puerto << endl;
    cout << "[MOTOR] TOP-K: " << topK << endl;
}

MotorBusqueda::~MotorBusqueda() {
    detener();
}

bool MotorBusqueda::cargarMapaLibros(const string& rutaMapa) {
    ifstream archivo(rutaMapa);
    if (!archivo.is_open()) {
        cerr << "[ERROR] No se pudo abrir el mapa de libros: " << rutaMapa << endl;
        return false;
    }
    
    string linea;
    // Saltar encabezado
    getline(archivo, linea);
    
    while (getline(archivo, linea)) {
        stringstream ss(linea);
        string idStr, nombre;
        
        if (getline(ss, idStr, ',') && getline(ss, nombre)) {
            try {
                int id = stoi(idStr);
                mapaLibros[id] = nombre;
            } catch (...) {
                cerr << "[ADVERTENCIA] Línea inválida: " << linea << endl;
            }
        }
    }
    
    archivo.close();
    cout << "[INFO] Mapa de libros cargado: " << mapaLibros.size() << " libros" << endl;
    return true;
}

bool MotorBusqueda::cargarIndiceInvertido(const string& rutaIdx) {
    ifstream archivo(rutaIdx);
    if (!archivo.is_open()) {
        cerr << "[ERROR] No se pudo abrir índice invertido: " << rutaIdx << endl;
        return false;
    }
    
    cout << "[INFO] Cargando índice invertido desde: " << rutaIdx << endl;
    
    // Formato del .idx (texto CSV):
    // palabra;(idLibro,frecuencia);(idLibro,frecuencia);...
    // Ejemplo: vampire;(35,1);(17,1);(19,28);(5,1)
    
    string linea;
    int numEntradas = 0;
    
    while (getline(archivo, linea)) {
        if (linea.empty()) continue;
        
        // Separar palabra y datos
        size_t posPuntoYComa = linea.find(';');
        if (posPuntoYComa == string::npos) continue;
        
        string palabra = linea.substr(0, posPuntoYComa);
        string datos = linea.substr(posPuntoYComa + 1);
        
        // Parsear todos los pares (idLibro,frecuencia)
        size_t pos = 0;
        while (pos < datos.length()) {
            size_t posParenAbre = datos.find('(', pos);
            if (posParenAbre == string::npos) break;
            
            size_t posComa = datos.find(',', posParenAbre);
            if (posComa == string::npos) break;
            
            size_t posParenCierra = datos.find(')', posComa);
            if (posParenCierra == string::npos) break;
            
            try {
                int idLibro = stoi(datos.substr(posParenAbre + 1, posComa - posParenAbre - 1));
                int frecuencia = stoi(datos.substr(posComa + 1, posParenCierra - posComa - 1));
                
                InfoLibro info;
                info.frecuencia = frecuencia;
                
                indiceInvertido[palabra][idLibro] = info;
                numEntradas++;
                
                pos = posParenCierra + 1;
            } catch (...) {
                break;
            }
        }
    }
    
    archivo.close();
    cout << "[INFO] Índice invertido cargado: " << numEntradas << " entradas" << endl;
    cout << "[INFO] Palabras únicas: " << indiceInvertido.size() << endl;
    return !indiceInvertido.empty();
}

void MotorBusqueda::cargarRecursos(const string& rutaIdx, const string& rutaMapa) {
    cout << "[INFO] Cargando recursos..." << endl;
    cargarMapaLibros(rutaMapa);
    cargarIndiceInvertido(rutaIdx);
    cout << "[INFO] Recursos cargados correctamente" << endl;
}

vector<string> MotorBusqueda::separarPalabras(const string& consulta) {
    vector<string> palabras;
    stringstream ss(consulta);
    string palabra;
    
    while (ss >> palabra) {
        // Convertir a minúsculas
        transform(palabra.begin(), palabra.end(), palabra.begin(), ::tolower);
        palabras.push_back(palabra);
    }
    
    return palabras;
}

vector<ResultadoScore> MotorBusqueda::calcularScores(const vector<string>& palabras) {
    // Mapa: idLibro -> score base
    map<int, double> scoresLibros;
    
    // Mapa: idLibro -> cantidad de palabras diferentes encontradas
    map<int, int> palabrasEncontradas;
    
    cout << "[BUSQUEDA] Buscando palabras: ";
    for (const auto& p : palabras) cout << "'" << p << "' ";
    cout << endl;
    
    // Para cada palabra de la consulta, buscar la forma exacta y la plural
    for (const string& palabra : palabras) {
        set<string> formasBusqueda;
        formasBusqueda.insert(palabra);
        formasBusqueda.insert(palabra + "s");

        bool algunaFormaEncontrada = false;
        
        for (const string& forma : formasBusqueda) {
            auto it = indiceInvertido.find(forma);
            if (it != indiceInvertido.end()) {
                algunaFormaEncontrada = true;
                cout << "[INFO] Palabra '" << forma << "' encontrada en " 
                     << it->second.size() << " libro(s)" << endl;
                
                for (const auto& [idLibro, info] : it->second) {
                    scoresLibros[idLibro] += info.frecuencia;
                    // Se incrementa solo una vez por palabra original de la consulta
                }
            }
        }

        // Marcar si alguna de las formas de la palabra se encontró en un libro
        if (algunaFormaEncontrada) {
            // Para actualizar palabrasEncontradas, necesitamos saber qué libros contenían la palabra
            // Este es un enfoque simplificado: si se encontró, asumimos que contribuyó a los libros con score.
            for (auto const& [idLibro, score] : scoresLibros) {
                 // Para evitar contar múltiples veces si un libro tiene ambas formas (singular y plural)
                 // y para contar solo si el score aumentó en este ciclo, necesitaríamos una lógica más compleja.
                 // Por ahora, lo más simple es iterar sobre los libros que tienen la palabra.
            }
             // Una forma más directa de manejar `palabrasEncontradas`
            set<int> librosConPalabra;
            for (const string& forma : formasBusqueda) {
                auto it = indiceInvertido.find(forma);
                if (it != indiceInvertido.end()) {
                    for (const auto& [idLibro, info] : it->second) {
                        librosConPalabra.insert(idLibro);
                    }
                }
            }
            for (int idLibro : librosConPalabra) {
                palabrasEncontradas[idLibro]++;
            }
        }

        if (!algunaFormaEncontrada) {
            cout << "[INFO] Palabra '" << palabra << "' (ni su plural) no encontrada." << endl;
        }
    }
    
    // Aplicar bonus del 30% a libros que tengan TODAS las palabras
    int numPalabras = palabras.size();
    for (auto& [idLibro, score] : scoresLibros) {
        if (palabrasEncontradas[idLibro] == numPalabras && numPalabras > 1) {
            score *= 1.3;
            cout << "[BONUS] Libro ID " << idLibro << " tiene todas las palabras (+30%)" << endl;
        }
    }
    
    // Convertir a vector de ResultadoScore
    vector<ResultadoScore> resultados;
    for (const auto& [idLibro, score] : scoresLibros) {
        ResultadoScore resultado;
        resultado.nombreLibro = mapaLibros[idLibro];
        resultado.score = score;
        resultados.push_back(resultado);
    }
    
    // Ordenar por score descendente
    sort(resultados.begin(), resultados.end(), greater<ResultadoScore>());
    
    // Limitar a TOP-K
    if (resultados.size() > (size_t)topK) {
        resultados.resize(topK);
    }
    
    cout << "[RESULTADOS] Top " << resultados.size() << " libros encontrados" << endl;
    
    return resultados;
}

string MotorBusqueda::generarJSON(const vector<ResultadoScore>& resultados) {
    stringstream json;
    json << "[";
    
    for (size_t i = 0; i < resultados.size(); i++) {
        json << "{\"libro\":\"" << resultados[i].nombreLibro << "\","
             << "\"score\":" << resultados[i].score << "}";
        
        if (i < resultados.size() - 1) {
            json << ",";
        }
    }
    
    json << "]";
    return json.str();
}

string MotorBusqueda::buscar(const vector<string>& palabras) {
    if (palabras.empty()) {
        return "[]";
    }
    
    vector<ResultadoScore> resultados = calcularScores(palabras);
    return generarJSON(resultados);
}

bool MotorBusqueda::inicializarSocket() {
    socketServidor = socket(AF_INET, SOCK_STREAM, 0);
    if (socketServidor < 0) {
        cerr << "[ERROR] No se pudo crear el socket" << endl;
        return false;
    }
    
    int opt = 1;
    if (setsockopt(socketServidor, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        cerr << "[ADVERTENCIA] No se pudo configurar SO_REUSEADDR" << endl;
    }
    
    struct sockaddr_in direccion;
    direccion.sin_family = AF_INET;
    direccion.sin_addr.s_addr = INADDR_ANY;
    direccion.sin_port = htons(puerto);
    
    if (bind(socketServidor, (struct sockaddr*)&direccion, sizeof(direccion)) < 0) {
        cerr << "[ERROR] No se pudo hacer bind en el puerto " << puerto << endl;
        close(socketServidor);
        return false;
    }
    
    if (listen(socketServidor, 10) < 0) {
        cerr << "[ERROR] Error al poner el socket en modo escucha" << endl;
        close(socketServidor);
        return false;
    }
    
    cout << "[INFO] Socket iniciado en puerto " << puerto << endl;
    return true;
}

void MotorBusqueda::manejarCliente(int socketCliente) {
    char buffer[1024] = {0};
    int bytesRecibidos = recv(socketCliente, buffer, sizeof(buffer) - 1, 0);
    
    if (bytesRecibidos <= 0) {
        close(socketCliente);
        return;
    }
    
    string consulta(buffer, bytesRecibidos);
    cout << "\n[CONSULTA] Recibida: '" << consulta << "'" << endl;
    
    // Parsear consulta (formato: palabra1 palabra2|archivo.idx)
    size_t separador = consulta.find('|');
    string palabrasStr = consulta;
    if (separador != string::npos) {
        palabrasStr = consulta.substr(0, separador);
        // El archivo.idx se ignora porque ya está cargado
    }
    
    vector<string> palabras = separarPalabras(palabrasStr);
    string respuestaJSON = buscar(palabras);
    
    cout << "[RESPUESTA] " << respuestaJSON << endl;
    
    send(socketCliente, respuestaJSON.c_str(), respuestaJSON.length(), 0);
    close(socketCliente);
}

void MotorBusqueda::iniciar() {
    if (!inicializarSocket()) {
        cerr << "[ERROR] No se pudo inicializar el servidor" << endl;
        return;
    }
    
    cout << "[INFO] Servidor MOTOR_BUSQUEDA escuchando en puerto " << puerto << endl;
    cout << "[INFO] Esperando conexiones..." << endl;
    cout << "[INFO] Presione Ctrl+C para detener el servidor" << endl;
    
    while (true) {
        struct sockaddr_in direccionCliente;
        socklen_t tamañoDireccion = sizeof(direccionCliente);
        
        int socketCliente = accept(socketServidor, 
                                   (struct sockaddr*)&direccionCliente, 
                                   &tamañoDireccion);
        
        if (socketCliente < 0) {
            cerr << "[ERROR] Error al aceptar conexión" << endl;
            continue;
        }
        
        char ipCliente[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &direccionCliente.sin_addr, ipCliente, INET_ADDRSTRLEN);
        cout << "\n[CONEXION] Cliente conectado desde " << ipCliente 
             << ":" << ntohs(direccionCliente.sin_port) << endl;
        
        manejarCliente(socketCliente);
    }
}

void MotorBusqueda::detener() {
    if (socketServidor >= 0) {
        close(socketServidor);
        socketServidor = -1;
        cout << "[INFO] Servidor MOTOR_BUSQUEDA detenido" << endl;
    }
}
