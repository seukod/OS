#include <iostream>
#include <limits>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include "Users.h"
#include "Menu.h"

using namespace std;

void agregarUser(vector<User>& lista);
void listarUsers(const vector<User>& lista);

bool validarUsuario(const string& inUser, const string& inPass, Usuario& userEncontrado) {
    ifstream archivo("USUARIOS.txt");//POR QUE NO ME ABRE EL ARCHIVOOOOOOOO
    if (!archivo.is_open()) {
        cout << "Error: no se pudo abrir el archivo USUARIOS.TXT" << endl;
        return false;
    }

    string linea;
    while (getline(archivo, linea, ';')) {
        if (linea.empty()) continue;

        stringstream datos(linea);
        Usuario u;
        getline(datos, u.id, ',');
        getline(datos, u.nombre, ',');
        getline(datos, u.username, ',');
        getline(datos, u.password, ',');
        getline(datos, u.perfil, ',');

        if (u.username == inUser && u.password == inPass) {
            userEncontrado = u;
            return true;
        }
    }
    return false;
}


void menu(const Usuario& user) {
    vector<User> lista_users;
    int opcion = 0;
    while (true) {
        cout << "\n===== MENU =====" << endl;
        cout << "1. Modulo Admin" << endl;
        cout << "2. Modulo General" << endl;
        cout << "3. Modulo por implementar " << endl;
        cout << "0. Salir" << endl;
        cout << "Elige una opcion: ";
        while (!(cin >> opcion)) {
            cout << "Entrada no valida, Intenta denuevo" << endl;
            cin.clear(); // limpia estado de error
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // limpia buffer de cin
        }

        if (opcion == 0) {
            cout << "Saliendo del programa..." << endl;
            break;
        }
        if (opcion == 1) {
            int admin_opcion = 0;
            do {
                cout << "\n===== Modulo ADMIN =====" << endl;
                cout << "1. Crear Usuario" << endl;
                cout << "2. Eliminar Usuario" << endl;
                cout << "3. Listar Usuarios" << endl;
                while (!(cin >> admin_opcion)) {
                    cout << "Entrada no valida, Intenta denuevo" << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
                if (admin_opcion == 1) {
                    char continuar = 'n';
                    do{
                        agregarUser(lista_users);
                        cout << "Desea agregar otro usuario? (s/n): ";
                        cin >> continuar;
                    } while (continuar == 's' || continuar == 'S');
                }
                if (admin_opcion == 2) {
                    // por implementar
                }
                if (admin_opcion == 3) {
                    listarUsers(lista_users);
                }
            }while (admin_opcion != 0);

        }
        if (opcion == 2) {
            // user
        }
        if (opcion == 3) {
            //to do
        }
    }
}

int main() {
    string username, password;
    Usuario user;
    bool loginExitoso = false;

    cout << "==============================" << endl;
    cout << "         INICIAR SESION       " << endl;
    cout << "==============================" << endl;

    while (!loginExitoso) {
        cout << "\nUsername: ";
        getline(cin, username);

        cout << "Password: ";
        getline(cin, password);

        if (validarUsuario(username, password, user)) {
            cout << "\n✅ Inicio de sesión exitoso." << endl;
            cout << "Bienvenido, " << user.nombre << endl;
            loginExitoso = true;

            // Llamar al menú único, pasándole el usuario
            menu(user);

        } else {
            cout << "\n❌ Usuario o contraseña incorrectos. Intente nuevamente." << endl;
        }
    }

    return 0;
}