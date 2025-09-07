#include "../../include/menus/menu_principal.h"
#include "../../include/menus/menu_admin.h"
#include "../../include/interfaz.h"
#include <iostream>
#include <algorithm>

using namespace std;

void mostrarMenuPrincipal(const Usuario& usuario) {
    limpiarPantalla();
    cout << "=================================================" << endl;
    cout << "                MENÚ PRINCIPAL                   " << endl;
    cout << "=================================================" << endl;
    cout << "Usuario: " << usuario.nombre << " (" << usuario.username << ")" << endl;
    cout << "Perfil: " << usuario.perfil << endl;
    cout << "=================================================" << endl;

    // Buscar el perfil del usuario en g_perfiles
    const vector<int>* permisos = nullptr;
    for (const auto& perfil : g_perfiles) {
        if (perfil.nombre == usuario.perfil) {
            permisos = &perfil.permisos;
            break;
        }
    }

    if (!permisos) {
        cout << "[ERROR] No se encontró el perfil '" << usuario.perfil << "'. Mostrando solo opción de salida." << endl;
        cout << "0. Salir" << endl;
        cout << "=================================================" << endl;
        cout << "Seleccione una opción: ";
        return;
    }

    // Mostrar solo las opciones permitidas según los permisos
    for (int opcion : *permisos) {
        switch (opcion) {
            case 0:
                cout << "0. Salir" << endl;
                break;
            case 1:
                cout << "1. Admin Users" << endl;
                break;
            case 2:
                cout << "2. Multi Matrices NxN" << endl;
                break;
            case 3:
                cout << "3. Juego" << endl;
                break;
            case 4:
                cout << "4. Es palindromo?" << endl;
                break;
            case 5:
                cout << "5. Calcula f(x)" << endl;
                break;
            case 6:
                cout << "6. Conteo sobre texto" << endl;
                break;
            default:
                cout << opcion << ". [OPCIÓN NO DEFINIDA]" << endl;
                break;
        }
    }

    cout << "=================================================" << endl;
    cout << "Seleccione una opción: ";
}

int leerOpcionPrincipal() {
    int opcion;
    while(!(cin >> opcion)) {
        mostrarMensajeError("Entrada inválida. Por favor ingrese un número.");
        limpiarBuffer();
        cout << "Seleccione una opción: ";
    }
    limpiarBuffer();
    return opcion;
}

void mostrarEnConstruccion(const string& funcionalidad) {
    limpiarPantalla();
    cout << "=================================================" << endl;
    cout << "              EN CONSTRUCCIÓN                    " << endl;
    cout << "=================================================" << endl;
    cout << "La funcionalidad '" << funcionalidad << "' está en construcción." << endl;
    cout << "Será implementada en futuras entregas." << endl;
    cout << "=================================================" << endl;
    pausarPantalla();
}

void ejecutarMenuPrincipal(const Usuario& usuario, const string& libro) {
    cout << "\n✅ Inicio de sesión exitoso." << endl;
    cout << "Bienvenido, " << usuario.nombre << endl;
    cout << "Libro seleccionado: " << libro << endl;
    pausarPantalla();

    int opcion;
    do {
        mostrarMenuPrincipal(usuario);
        opcion = leerOpcionPrincipal();

        // Validar si la opción está dentro de los permisos del perfil
        bool permitido = false;
        for (const auto& perfil : g_perfiles) {
            if (perfil.nombre == usuario.perfil) {
                if (find(perfil.permisos.begin(), perfil.permisos.end(), opcion) != perfil.permisos.end()) {
                    permitido = true;
                }
                break;
            }
        }

        if (!permitido) {
            mostrarMensajeError("No tienes permiso para acceder a esta opción.");
            pausarPantalla();
            continue;
        }

        // Ejecutar opción
        switch(opcion) {
            case 0:
                mostrarMensajeDespedida();
                break;
            case 1:
                ejecutarMenuAdmin(usuario);
                break;
            case 2:
                mostrarEnConstruccion("Multi Matrices NxN");
                break;
            case 3:
                mostrarEnConstruccion("Juego");
                break;
            case 4:
                mostrarEnConstruccion("Es palindromo?");
                break;
            case 5:
                mostrarEnConstruccion("Calcula f(x)");
                break;
            case 6:
                mostrarEnConstruccion("Conteo sobre texto");
                break;
            default:
                mostrarMensajeError("Opción inválida.");
                pausarPantalla();
                break;
        }
    } while(opcion != 0);
}

