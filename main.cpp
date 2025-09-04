#include <iostream>
#include <limits>
<<<<<<< Updated upstream
#include "UI_Func.h"
=======
<<<<<<< HEAD
#include "interfaz.h"
#include "usuarios.h"
#include "crud.h"
#include "ArgParser.h"
#include "AppConfig.h"

using namespace std;

// Función que contiene el menú principal
void menu(const Usuario& user) {
    mostrarBienvenida();
    cout << "Usuario autenticado: " << user.nombre << endl;

    int opcion;
    do {
        mostrarMenuPrincipal();
        opcion = leerOpcion();

        switch(opcion){
            case 0:
                mostrarMensajeDespedida();
                break;
            case 1:
                ingresarUsuario();
                break;
            case 2:
                eliminarUsuario();
                break;
            case 3:
                listarUsuarios();
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
=======

>>>>>>> Stashed changes
#include "Funciones_usuarios.h"



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
				ingresarUsuario();
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

	return 0;
}
