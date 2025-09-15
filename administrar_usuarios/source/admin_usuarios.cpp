#include "../include/admin_usuarios.h"

#include <unistd.h>

int main() {

    mostrarTituloAplicacion();

    // Cargar datos en memoria al iniciar
    if (!cargarDatosEnMemoria()) {
        mostrarMensajeError("Error al cargar datos. Continuando con datos vacíos.");
    }

    ejecutarAplicacion();

    // Guardar cambios antes de salir
    if (guardarCambios()) {
        mostrarMensajeExito("Cambios guardados exitosamente.");
    } else {
        mostrarMensajeError("Error al guardar cambios.");
    }

    mostrarMensajeDespedida();
    return 0;
}

void mostrarTituloAplicacion() {
    limpiarPantalla();
    cout << "Aplicación administrador iniciada, PID: " << getpid() << endl;
    mostrarTitulo("ADMINISTRADOR DE USUARIOS");
    cout << "Aplicación independiente para gestión de usuarios y perfiles" << endl;
    mostrarSeparador();
    pausarPantalla();
}

void mostrarMenuPrincipal() {
    limpiarPantalla();
    mostrarTitulo("MENÚ PRINCIPAL");
    cout << "1. Ingresar Usuario" << endl;
    cout << "2. Eliminar Usuario" << endl;
    cout << "3. Listar Usuarios" << endl;
    cout << "4. Ingresar Perfil" << endl;
    cout << "5. Eliminar Perfil" << endl;
    cout << "6. Listar Perfiles" << endl;
    cout << "7. Guardar Cambios" << endl;
    cout << "0. Salir" << endl;
    mostrarSeparador();
    cout << "Seleccione una opción: ";
}

void ejecutarAplicacion() {
    int opcion;
    do {
        mostrarMenuPrincipal();
        opcion = leerOpcion();

        procesarOpcionMenu(opcion);

        if (opcion != 0) {
            pausarPantalla();
        }
    } while (opcion != 0);
}

void procesarOpcionMenu(int opcion) {
    switch(opcion) {
        case 0:
            cout << "Saliendo de la aplicación..." << endl;
            break;
        case 1:
            ingresarUsuarioEnMemoria();
            break;
        case 2:
            eliminarUsuarioEnMemoria();
            break;
        case 3:
            listarUsuarios();
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
}
