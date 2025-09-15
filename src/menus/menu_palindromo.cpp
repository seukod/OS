#include "../../include/menus/menu_palindromo.h"
#include "../../include/interfaz.h"
#include "../../include/utils/input_utils.h"
#include <iostream>
#include <unistd.h>
#include <algorithm>
#include <cctype>

using namespace std;

void mostrarMenuPalindromo() {
    limpiarPantalla();
    cout << "=================================================" << endl;
    cout << "            VERIFICADOR DE PALÍNDROMOS           " << endl;
    cout << "=================================================" << endl;
    cout << "Ingrese el texto que desea verificar:" << endl;
    cout << "(Los espacios, tildes y signos serán ignorados)" << endl;
    cout << "=================================================" << endl;
}

void ejecutarMenuPalindromo() {
    cout << "PID DEL PROCESO: " << getpid() << endl;
    string texto;
    int opcion;
    
    do {
        mostrarMenuPalindromo();
        
        // Solicitar texto al usuario
        cout << "Texto: ";
        if (!getline(cin >> ws, texto)) {
            mostrarMensajeError("Error al leer el texto.");
            return;
        }
        
        // Verificar si el usuario quiere cancelar
        if (texto.empty()) {
            cout << "No se ingresó texto. Volviendo al menú anterior..." << endl;
            pausarPantalla();
            return;
        }
        
        // Mostrar el menú de opciones
        limpiarPantalla();
        cout << "=================================================" << endl;
        cout << "            VERIFICADOR DE PALÍNDROMOS           " << endl;
        cout << "=================================================" << endl;
        cout << "Texto ingresado: \"" << texto << "\"" << endl;
        cout << "=================================================" << endl;
        cout << "1. Verificar si es palíndromo" << endl;
        cout << "2. Cancelar y volver atrás" << endl;
        cout << "=================================================" << endl;
        cout << "Seleccione una opción: ";
        
        opcion = leerOpcion();
        
        switch(opcion) {
            case 1: {
                // Verificar palíndromo
                bool resultado = esPalindromo(texto);
                string textoLimpio = limpiarTexto(texto);
                
                limpiarPantalla();
                cout << "=================================================" << endl;
                cout << "            RESULTADO DE VERIFICACIÓN           " << endl;
                cout << "=================================================" << endl;
                cout << "Texto original: \"" << texto << "\"" << endl;
                cout << "Texto procesado: \"" << textoLimpio << "\"" << endl;
                cout << "=================================================" << endl;
                
                if (resultado) {
                    cout << "¡ES UN PALÍNDROMO!" << endl;
                    cout << "El texto se lee igual de izquierda a derecha" << endl;
                    cout << "y de derecha a izquierda." << endl;
                } else {
                    cout << "NO ES UN PALÍNDROMO" << endl;
                    cout << "El texto no se lee igual en ambas direcciones." << endl;
                }
                
                cout << "=================================================" << endl;
                pausarPantalla();
                return; // Volver al menú principal después de mostrar el resultado
            }
            case 2:
                cout << "Operación cancelada. Volviendo al menú anterior..." << endl;
                pausarPantalla();
                return;
            default:
                mostrarMensajeError("Opción inválida. Seleccione 1 o 2.");
                pausarPantalla();
                break;
        }
    } while(opcion != 1 && opcion != 2);
}

// Recorre char a char del texto y verifica si es alfanumerico y si es, lo agrega al string nuevo
string limpiarTexto(const string& texto) {
    string limpio;
    for (char c : texto) {
        if (isalnum(c)) {
            limpio += tolower(c);
        }
    }
    return limpio;
}

// Verifica si es palindromo; 
bool esPalindromo(const string& texto) {
    // Se limpia el texto
    string textoLimpio = limpiarTexto(texto);
    
    if (textoLimpio.empty()) {
        return false;
    }
    
    int inicio = 0;
    int fin = textoLimpio.length() - 1;

    // Mientras sean iguales se suma el int inicio y se quita al final
    while (inicio < fin) {
        if (textoLimpio[inicio] != textoLimpio[fin]) {
            return false;
        }
        inicio++;
        fin--;
    }
    
    return true;
}