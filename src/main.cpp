#include <iostream>
#include <exception>
#include <limits>
#include "../include/interfaz.h"
#include "../include/usuarios.h"
#include "../include/users_auth.h"
#include "../include/ArgParser.h"
#include "../include/AppConfig.h"
#include "../include/menus/menu_principal.h"

using namespace std;

int main(int argc, char* argv[]) {
    try {
        // 1) Limpiar memoria y cargar datos de usuarios y perfiles al inicio
        cout << "[INFO] Iniciando sistema..." << endl;
        if (!cargarDatosEnMemoria()) {
            cerr << "[ERROR] No se pudieron cargar los datos en memoria. Terminando programa." << endl;
            return 1;
        }
        
        // 2) Parseo y validaciones de CLI (Command Line Interface)
        Options opts = parseArguments(argc, argv);

        // 3) Usar las credenciales de CLI para validar usuario
        string username = opts.user;
        string password = opts.password;
        string bookName = opts.book;

        Usuario user;
        if (validarUsuario(username, password, user)) {
            // Llamar al nuevo menú principal
            ejecutarMenuPrincipal(user, bookName);
            return 0;
        } else {
            cerr << "\n Usuario o contraseña incorrectos.\n";
            return 1;
        }
    }
    catch (const exception& e) {
        cerr << e.what() << endl;
        cerr << "Use -h o --help para ver la ayuda.\n";
        return 1;
    }
}