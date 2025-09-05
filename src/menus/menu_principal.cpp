#include "../../include/menus/menu_principal.h"
#include "../../include/menus/menu_admin.h"
#include "../../include/interfaz.h"
#include <iostream>
#include <limits>

using namespace std;

void mostrarMenuPrincipal(const Usuario& usuario) {
    limpiarPantalla();
    cout << "=================================================" << endl;
    cout << "                MENÚ PRINCIPAL                   " << endl;
    cout << "=================================================" << endl;
    cout << "Usuario: " << usuario.nombre << " (" << usuario.username << ")" << endl;
    cout << "=================================================" << endl;
    cout << "0. Salir" << endl;
    cout << "1. Admin Users" << endl;
    cout << "2. Multi Matrices NxN" << endl;
    cout << "3. Juego" << endl;
    cout << "4. Es palindromo?" << endl;
    cout << "5. Calcula f(x)" << endl;
    cout << "6. Conteo sobre texto" << endl;
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

        switch(opcion) {
            case 0:
                mostrarMensajeDespedida();
                break;
            case 1:
                // Llamar al menú de administración existente
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
                mostrarMensajeError("Opción inválida. Seleccione una opción válida.");
                pausarPantalla();
                break;
        }
    } while(opcion != 0);
}
