#ifndef MENU_CONTEO_H
#define MENU_CONTEO_H

#include <string>

using namespace std;

// Estructura para almacenar los resultados del conteo
struct ConteoResultado {
    int vocales;
    int consonantes;
    int caracteresEspeciales;
    int palabras;
    int totalCaracteres;
};

// Funciones principales del menú de conteo
void ejecutarMenuConteo(const string& nombreLibro);
void mostrarMenuConteo();
void mostrarResultadosConteo(const ConteoResultado& resultado, const string& nombreLibro);

// Funciones de conteo
ConteoResultado contarElementosTexto(const string& texto);
string leerLibro(const string& nombreLibro);
bool esVocal(char c);
bool esConsonante(char c);
bool esCaracterEspecial(char c);

#endif // MENU_CONTEO_H