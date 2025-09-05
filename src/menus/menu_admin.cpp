#include "../../include/menus/menu_admin.h"
#include "../../include/interfaz.h"
#include "../../include/usuarios.h"
#include "../../include/crud.h"
#include <iostream>
#include <limits>

using namespace std;

void mostrarMenuAdmin() {
    limpiarPantalla();
    cout << "=================================================" << endl;
    cout << "              ADMINISTRACIÓN                     " << endl;
    cout << "=================================================" << endl;
    cout << "1. Ingresar Usuario" << endl;
    cout << "2. Eliminar Usuario" << endl;
    cout << "3. Listar Usuarios" << endl;
    cout << "4. Ingresar Perfil" << endl;
    cout << "5. Eliminar Perfil" << endl;
    cout << "6. Listar Perfiles" << endl;
    cout << "7. Guardar Cambios" << endl;
    cout << "0. Volver al Menú Principal" << endl;
    cout << "=================================================" << endl;
    cout << "Seleccione una opción: ";
}

int leerOpcionAdmin() {
    int opcion;
    while(!(cin >> opcion)) {
        mostrarMensajeError("Entrada inválida. Por favor ingrese un número.");
        limpiarBuffer();
        cout << "Seleccione una opción: ";
    }
    limpiarBuffer();
    return opcion;
}

void ejecutarMenuAdmin(const Usuario& usuario) {
    mostrarTitulo("MODO ADMINISTRACIÓN");
    cout << "Usuario administrador: " << usuario.nombre << endl;
    pausarPantalla();

    int opcion;
    do {
        mostrarMenuAdmin();
        opcion = leerOpcionAdmin();

        switch(opcion) {
            case 0:
                cout << "Volviendo al menú principal..." << endl;
                pausarPantalla();
                break;
            case 1:
                ingresarUsuarioEnMemoria();
                break;
            case 2:
                eliminarUsuarioEnMemoria();
                break;
            case 3:
                listarUsuarios(g_usuarios);
                break;
            case 4:
                ingresarPerfilEnMemoria();
                break;
            case 5:
                eliminarPerfilEnMemoria();
                break;
            case 6:
                listarPerfilesEnMemoria();
                break;
            case 7:
                if (guardarCambios()) {
                    mostrarMensajeExito("Cambios guardados exitosamente.");
                } else {
                    mostrarMensajeError("Error al guardar cambios.");
                }
                break;
            default:
                mostrarMensajeError("Opción inválida. Seleccione una opción válida.");
                break;
        }

        if(opcion != 0) {
            pausarPantalla();
        }
    } while(opcion != 0);
}
