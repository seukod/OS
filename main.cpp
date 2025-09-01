#include <iostream>
#include <limits>
#include "UI_Func.h"
#include "UsersFunc.h"
#include "centro.h" //aqui donde se leen los archivo .txt


using namespace std;

int main(){
	mostrarBienvenida();

	string rutaUsuarios = getUserFilePath(".env");
	vector<Usuario> usuarios = leerUsuarios(rutaUsuarios);

	int opcion;
	do {
		mostrarMenuPrincipal();
		opcion = leerOpcion();

		switch(opcion){
			case 0:
				mostrarMensajeDespedida();
				break;
			case 1:
				ingresarUsuario(usuarios,rutaUsuarios);
				break;
			case 2:
				eliminarUsuario(usuarios,rutaUsuarios);
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
