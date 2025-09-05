#include <iostream>
#include <limits>
#include "interfaz.h"
#include "usuarios.h"
#include "crud.h"
#include "ArgParser.h"
#include "AppConfig.h"

using namespace std;

int main(){
    mostrarBienvenida();
    vector<Usuario> usuarios = crear_arreglo();
    int opcion;
    do {
        mostrarMenuPrincipal();
        opcion = leerOpcion();

        switch(opcion){
        case 0:
            mostrarMensajeDespedida();
            break;
        case 1:
            ingresarUsuario(usuarios);
            break;
        case 2:
            eliminarUsuario(usuarios);
            break;
        case 3:
            listarUsuarios(usuarios);
            break;
        case 4:
            ingresarPerfil();
            break;
        case 5:
            eliminarPerfil();
            break;
        case 6:
            listarPerfiles();
            break;
        default:
            mostrarMensajeError("Opción inválida. Seleccione una opción válida.");
            break;
        }

        if(opcion != 0){
            pausarPantalla();
        }
    }while(opcion != 0);
    guardar_cambios(usuarios);
    return 0;
}
