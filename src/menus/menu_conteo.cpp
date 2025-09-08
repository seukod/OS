#include "../../include/menus/menu_conteo.h"
#include "../../include/interfaz.h"
#include "../../include/utils/input_utils.h"
#include <iostream>
#include <fstream>
#include <cctype>
#include <limits>
#include <cstdlib>  // para getenv
using namespace std;

string leerVariableEnv(const string& nombre) {
    const char* valor = getenv(nombre.c_str());
    return (valor != nullptr) ? string(valor) : "";
}

// Función optimizada para contar desde un archivo
void contarArchivo(const string& filePath, int& vocales, int& consonantes, int& especiales, int& palabras) {
    vocales = consonantes = especiales = palabras = 0;
    bool enPalabra = false;
    string ingreso = leerVariableEnv("BOOKS_DIR") + filePath;
    ifstream file(ingreso);
    if (!file.is_open()) {
        cerr << "[ERROR] No se pudo abrir el archivo: " << ingreso << endl;
        return;
    };
    char c;
    while (file.get(c)) {
        if (isalpha(static_cast<unsigned char>(c))) {
            char lower = tolower(c);
            if (lower == 'a' || lower == 'e' || lower == 'i' || lower == 'o' || lower == 'u')
                vocales++;
            else
                consonantes++;
            enPalabra = true;
        }
        else if (isspace(static_cast<unsigned char>(c))) {
            if (enPalabra) {
                palabras++;
                enPalabra = false;
            }
        }
        else {
            especiales++;
        }
    }

    if (enPalabra) palabras++;
    file.close();
}

void mostrarInterfazConteo(const string& filePath) {
    limpiarPantalla();

    int vocales, consonantes, especiales, palabras;


    cout << "=================================================" << endl;
    cout << "              CONTEO SOBRE ARCHIVO               " << endl;
    cout << "Archivo: " << filePath << endl;
    cout << "=================================================" << endl;

    contarArchivo(filePath, vocales, consonantes, especiales, palabras);

    cout << "Cantidad de vocales:            " << vocales << endl;
    cout << "Cantidad de consonantes:        " << consonantes << endl;
    cout << "Cantidad de caracteres especiales: " << especiales << endl;
    cout << "Cantidad de palabras:           " << palabras << endl;
    cout << "-------------------------------------------------" << endl;
    cout << "0) VOLVER" << endl;
    cout << "=================================================" << endl;

    int opcion;
    cout << "Seleccione una opción: ";
    cin >> opcion;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}