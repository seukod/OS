#include "../include/crear_indice.h"
#include <fstream>
#include <dirent.h>
#include <sys/stat.h>
#include <cctype>

string limpiarPalabra(const string& termino) {
    if (termino.empty()) {
        return "";
    }
    return termino;
}

bool procesarDirectorio(const string& carpetaLibros, unordered_map<string, vector<DocumentoInfo>>& indiceInvertido) {
    DIR* punteroDir = opendir(carpetaLibros.c_str());
    if (!punteroDir) {
        cout << "ERROR: No se pudo abrir la carpeta: " << carpetaLibros << endl;
        return false;
    }

    cout << "Procesando archivos en: " << carpetaLibros << endl;

    struct dirent* entradaDir;
    int cantidadArchivos = 0;

    while ((entradaDir = readdir(punteroDir)) != nullptr) {
        string nombreDoc = entradaDir->d_name;

        if (nombreDoc == "." || nombreDoc == "..") {
            continue;
        }

        string rutaArchivo = carpetaLibros + "/" + nombreDoc;
        struct stat infoArchivo;
        if (stat(rutaArchivo.c_str(), &infoArchivo) == 0 && S_ISREG(infoArchivo.st_mode)) {
            if (nombreDoc.length() > 4 &&
                nombreDoc.substr(nombreDoc.length() - 4) == ".txt") {

                cout << "Procesando: " << nombreDoc << endl;
                procesarArchivo(nombreDoc, rutaArchivo, indiceInvertido);
                cantidadArchivos++;
            }
        }
    }

    closedir(punteroDir);

    cout << "Total de archivos procesados: " << cantidadArchivos << endl;
    return cantidadArchivos > 0;
}

void procesarArchivo(const string& docNombre, const string& docRuta, unordered_map<string, vector<DocumentoInfo>>& indiceInvertido) {
    ifstream lector(docRuta);
    if (!lector.is_open()) {
        cout << "No se pudo abrir el archivo: " << docNombre << endl;
        return;
    }

    unordered_map<string, int> conteoLocal;
    string lineaLeida;

    while (getline(lector, lineaLeida)) {
        string token = "";

        for (char caracter : lineaLeida) {
            if (isalpha(caracter)) {
                token += tolower(caracter);
            } else {
                if (!token.empty()) {
                    string limpio = limpiarPalabra(token);
                    if (!limpio.empty()) {
                        conteoLocal[limpio]++;
                    }
                    token = "";
                }
            }
        }

        if (!token.empty()) {
            string limpio = limpiarPalabra(token);
            if (!limpio.empty()) {
                conteoLocal[limpio]++;
            }
        }
    }

    lector.close();

    for (const auto& par : conteoLocal) {
        const string& termino = par.first;
        int repeticiones = par.second;
        auto& listaDocs = indiceInvertido[termino];
        listaDocs.emplace_back(docNombre, repeticiones);
    }
}

bool guardarIndice(const string& rutaSalida, const unordered_map<string, vector<DocumentoInfo>>& indiceInvertido) {
    ofstream escritor(rutaSalida);
    if (!escritor.is_open()) {
        cout << "ERROR: No se pudo crear el archivo: " << rutaSalida << endl;
        return false;
    }

    cout << "\nGuardando índice invertido en: " << rutaSalida << endl;

    for (const auto& par : indiceInvertido) {
        escritor << par.first;
        const vector<DocumentoInfo>& listaDocs = par.second;
        for (const DocumentoInfo& doc : listaDocs) {
            escritor << ";(" << doc.nombre << "," << doc.cantidad << ")";
        }
        escritor << endl;
    }

    escritor.close();
    cout << "Índice guardado exitosamente." << endl;
    return true;
}

bool crearIndice(const string& archivoDestino, const string& carpetaOrigen) {
    cout << "=================================================" << endl;
    cout << "         CREADOR DE ÍNDICE INVERTIDO             " << endl;
    cout << "=================================================" << endl;
    cout << "Carpeta de entrada: " << carpetaOrigen << endl;
    cout << "Archivo de salida: " << archivoDestino << endl;
    cout << "Proceso índice invertido iniciado, PID: " << getpid() << endl;
    cout << "=================================================" << endl;

    unordered_map<string, vector<DocumentoInfo>> indiceInvertido;

    if (!procesarDirectorio(carpetaOrigen, indiceInvertido)) {
        cout << "ERROR: No se pudieron procesar los archivos." << endl;
        return false;
    }

    if (!guardarIndice(archivoDestino, indiceInvertido)) {
        cout << "ERROR: No se pudo guardar el índice." << endl;
        return false;
    }

    return true;
}
