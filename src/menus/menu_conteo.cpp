#include "../../include/menus/menu_conteo.h"
#include "../../include/interfaz.h"
#include "../../include/utils/input_utils.h"
#include "../../include/users_auth.h"
#include <iostream>
#include <fstream>
#include <cctype>
#include <sstream>
#include <algorithm>

using namespace std;

void ejecutarMenuConteo(const string& nombreLibro) {
    int opcion;
    do {
        mostrarMenuConteo();
        opcion = leerOpcion();

        switch(opcion) {
            case 1: {
                // Realizar conteo del libro
                limpiarPantalla();
                cout << "=================================================" << endl;
                cout << "             CONTEO DE TEXTO                     " << endl;
                cout << "=================================================" << endl;
                cout << "Procesando el libro: " << nombreLibro << endl;
                cout << "Por favor espere..." << endl;

                string contenidoLibro = leerLibro(nombreLibro);
                if (contenidoLibro.empty()) {
                    mostrarMensajeError("No se pudo leer el libro especificado.");
                    pausarPantalla();
                    break;
                }

                ConteoResultado resultado = contarElementosTexto(contenidoLibro);
                mostrarResultadosConteo(resultado, nombreLibro);
                pausarPantalla();
                break;
            }
            case 0:
                cout << "\nVolviendo al menú principal..." << endl;
                break;
            default:
                mostrarMensajeError("Opción inválida. Por favor seleccione 1 o 0.");
                pausarPantalla();
                break;
        }
    } while(opcion != 0);
}

void mostrarMenuConteo() {
    limpiarPantalla();
    cout << "=================================================" << endl;
    cout << "             CONTEO SOBRE TEXTO                  " << endl;
    cout << "=================================================" << endl;
    cout << "1. Realizar conteo del libro" << endl;
    cout << "0. Volver al menú principal" << endl;
    cout << "=================================================" << endl;
    cout << "Seleccione una opción: ";
}

void mostrarResultadosConteo(const ConteoResultado& resultado, const string& nombreLibro) {
    limpiarPantalla();
    cout << "=================================================" << endl;
    cout << "           RESULTADOS DEL CONTEO                " << endl;
    cout << "=================================================" << endl;
    cout << "Libro: " << nombreLibro << endl;
    cout << "=================================================" << endl;
    cout << "CONTEO DEL TEXTO:" << endl;
    cout << "------------------------------------------------" << endl;
    cout << "• Vocales:              " << resultado.vocales << endl;
    cout << "• Consonantes:          " << resultado.consonantes << endl;
    cout << "• Caracteres especiales: " << resultado.caracteresEspeciales << endl;
    cout << "• Palabras:             " << resultado.palabras << endl;
    cout << "• Total de caracteres:  " << resultado.totalCaracteres << endl;
    cout << "=================================================" << endl;
    
    cout << "Presione Enter para continuar...";
}

ConteoResultado contarElementosTexto(const string& texto) {
    ConteoResultado resultado = {0, 0, 0, 0, 0};
    bool enPalabra = false;
    
    for (char c : texto) {
        resultado.totalCaracteres++;
        
        if (esVocal(c)) {
            resultado.vocales++;
        } else if (esConsonante(c)) {
            resultado.consonantes++;
        } else if (esCaracterEspecial(c)) {
            resultado.caracteresEspeciales++;
        }
        
        // Contar palabras (solo consideramos caracteres alfanuméricos básicos)
        if (isalnum(c)) {
            if (!enPalabra) {
                resultado.palabras++;
                enPalabra = true;
            }
        } else {
            enPalabra = false;
        }
    }
    
    return resultado;
}

string leerLibro(const string& nombreLibro) {
    // Leer la ruta de los libros desde el archivo .env
    string rutaLibros = leerVariableEnv("BOOKS_PATH", ".env");
    if (rutaLibros.empty()) {
        cerr << "[ERROR] No se pudo leer la variable BOOKS_PATH del archivo .env" << endl;
        return "";
    }
    
    // Construir la ruta completa del libro
    string rutaCompleta = rutaLibros + nombreLibro;
    
    ifstream archivo(rutaCompleta);
    if (!archivo.is_open()) {
        cerr << "[ERROR] No se pudo abrir el archivo." << endl;
        return "";
    }
    
    // Leer todo el contenido del archivo
    stringstream buffer;
    buffer << archivo.rdbuf();
    archivo.close();
    
    return buffer.str();
}

bool esVocal(char c) {
    char minuscula = tolower(c);
    return (minuscula == 'a' || minuscula == 'e' || minuscula == 'i' || 
            minuscula == 'o' || minuscula == 'u');
}

bool esConsonante(char c) {
    return isalpha(c) && !esVocal(c);
}

bool esCaracterEspecial(char c) {
    return !isalnum(c) && !esVocal(c) && !esConsonante(c);
}
