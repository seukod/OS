// Implementación de utilidades para manipulación de cadenas de texto
#include "../include/utils/string_utils.h"
#include <cctype>

using namespace std;

// Elimina espacios en blanco al inicio de una cadena (left trim)
void ltrim(string &s) { 
    while (!s.empty() && isspace((unsigned char) s.front())) 
        s.erase(s.begin()); 
}

// Elimina espacios en blanco al final de una cadena (right trim)
void rtrim(string &s) { 
    while (!s.empty() && isspace((unsigned char) s.back())) 
        s.pop_back(); 
}

// Elimina espacios en blanco al inicio y al final de una cadena
void trim(string &s) {
    ltrim(s);
    rtrim(s);
}

// Elimina comillas dobles o simples que rodean una cadena, si las tiene.
// Si la cadena no tiene comillas al inicio y final, la devuelve sin cambios.
string stripQuotes(string v) {
    if (v.size() >= 2 && ((v.front() == '"' && v.back() == '"') || (v.front() == '\'' && v.back() == '\''))) {
        return v.substr(1, v.size() - 2); // Devuelve la cadena sin el primer y último caracter
    }
    return v; // Devuelve la cadena original si no tiene comillas
}
