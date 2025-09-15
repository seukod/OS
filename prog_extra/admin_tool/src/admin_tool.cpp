// prog_extra/admin_tool/src/admin_tool.cpp
#include "../../../include/interfaz.h"
#include "../../../include/users_auth.h"
#include "../../../include/usuarios.h"
#include "../../../include/utils/input_utils.h"
#include <iostream>
#include <unistd.h>
#include <string>

using namespace std;

// -----------------------------
// Funciones auxiliares
// -----------------------------
void mostrarMenuAdminExterno() {
    limpiarPantalla();
    cout << "=================================================" << endl;
    cout << "       ADMINISTRACIÓN EXTERNA (APP SEPARADA)     " << endl;
    cout << "=================================================" << endl;
    cout << "1. Ingresar Usuario" << endl;
    cout << "2. Eliminar Usuario" << endl;
    cout << "3. Listar Usuarios" << endl;
    cout << "4. Ingresar Perfil" << endl;
    cout << "5. Eliminar Perfil" << endl;
    cout << "6. Listar Perfiles" << endl;
    cout << "7. Guardar Cambios" << endl;
    cout << "0. Salir" << endl;
    cout << "=================================================" << endl;
    cout << "Seleccione una opción: ";
}

// -----------------------------
// Función principal del menú externo
// -----------------------------
void ejecutarAdminToolExterno() {
    cout << "Admin Tool Externa - PID: " << getpid() << endl;

    if (!cargarDatosEnMemoria()) {
        cerr << "Error al cargar datos. Saliendo..." << endl;
        return;
    }

    int opcion;
    do {
        mostrarMenuAdminExterno();
        opcion = leerOpcion();

        switch (opcion) {
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
            case 0:
                cout << "Saliendo de la aplicación externa..." << endl;
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

// -----------------------------
// Main para ejecutar externamente
// -----------------------------
int main() {
    ejecutarAdminToolExterno();
    return 0;
}
