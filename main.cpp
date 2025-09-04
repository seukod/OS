#include <iostream>
#include <limits>
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
#include "UI_Func.h"
#include "Funciones_usuarios.h"
#include "Texto.h"


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
				eliminarUsuario();
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
>>>>>>> 45b4f14e04a0bc7d9a50496b575a3facc247a2ad

        if(opcion != 0){
            pausarPantalla();
        }
    } while(opcion != 0);
}

int main(int argc, char* argv[]) {
    // Parseo y validación de argumentos de línea de comandos
    Options opts = ArgParser::parse(argc, argv);

    // Configurar la ruta del archivo de usuarios de forma centralizada
    AppConfig::setUsuariosFile(opts.file);

    // Usar credenciales provistas por CLI
    string username = opts.user;
    string password = opts.password;

    Usuario user;
    if (validarUsuario(username, password, user)) {
        cout << "\n✅ Inicio de sesión exitoso." << endl;
        cout << "Bienvenido, " << user.nombre << endl;

        // Llamar al menú único, pasándole el usuario
        menu(user);
        return 0;
    } else {
        cerr << "\n❌ Usuario o contraseña incorrectos." << endl;
        return 1;
    }
}
