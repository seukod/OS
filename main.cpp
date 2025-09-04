#include <iostream>
#include <limits>
#include "interfaz.h"
#include "usuarios.h"
#include "crud.h"


using namespace std;

int main(){
	mostrarBienvenida();

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

		if(opcion != 0){
			pausarPantalla();
		}
	}while(opcion != 0);

	return 0;
}
