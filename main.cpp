#include <iostream>
#include <exception>
#include <limits>
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
                {
                    vector<Usuario> usuarios = crear_arreglo();
                    listarUsuarios(usuarios);
                }
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
    } while(opcion != 0);
}

int main(int argc, char* argv[]) {
    try {
        // 1) Parseo y validaciones de CLI
        Options opts = parseArguments(argc, argv);

        // 2) Configurar la ruta del archivo de usuarios
        setUsuariosFile(opts.file);

        // 3) Usar las credenciales de CLI
        string username = opts.user;
        string password = opts.password;

        Usuario user;
        if (validarUsuario(username, password, user)) {
            cout << "\n✅ Inicio de sesión exitoso.\n";
            cout << "Bienvenido, " << user.nombre << "\n";
            // Llamar al menu único, pasándole el usuario
            menu(user);
            return 0;
        } else {
            cerr << "\n❌ Usuario o contraseña incorrectos.\n";
            return 1;
        }
    }
    catch (const exception& e) {
        cerr << e.what() << endl;
        cerr << "Use -h o --help para ver la ayuda.\n";
        return 1;
    }
}