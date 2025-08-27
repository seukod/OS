#include <cstdlib>
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
    ifstream archivo("C:/Users/benja/Documents/GitHub/OS/USUARIOS.txt");//POR QUE NO ME ABRE EL ARCHIVOOOOOOOO
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

void listarUsuarios() {
    ifstream archivo("C:/Users/benja/Documents/GitHub/OS/USUARIOS.txt");  // Abrir archivo en modo lectura
    if (!archivo.is_open()) {
        cout << "Error: no se pudo abrir el archivo USUARIOS.txt" << endl;
        return;
    }

    string linea;
    while (getline(archivo, linea)) { // Leer línea completa
        stringstream ss(linea);
        string campo;
        int contador = 0;

        // Leer los campos separados por coma
        while (getline(ss, campo, ',')) {
            contador++;
            if (contador == 3) { // El tercer campo (username)
                // Quitar ; si está al fina
                cout << campo << endl;
                break; // ya no necesitamos leer más campos
            }
        }
    }

    archivo.close();
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
        while (!(cin >> opcion) || opcion < 0||opcion > 3) {
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
                            string usuario = "1"; //El ID esta hardcodeado por ahora
                            string entrada;
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');//Limpia el buffer
                            cout << "ingrese nombre: ";
                            getline(cin, entrada);
                            usuario += "," + entrada;
                            cout << "ingrese username: ";
                            getline(cin, entrada);
                            usuario += "," + entrada;
                            cout << "ingrese password: ";
                            getline(cin, entrada);
                            usuario += "," + entrada;
                            cout << "ingrese perfil: ";
                            getline(cin, entrada);
                            usuario += "," + entrada + ";";
                            // Abrimos el archivo en modo append
                            ofstream archivo("C:/Users/benja/Documents/GitHub/OS/USUARIOS.txt", ios::app);
                            if (!archivo.is_open()) {
                                cout << "Error: no se pudo abrir el archivo USUARIOS.txt" << endl;
                            }
                            archivo << usuario << "\n";archivo.close(); // cerramos el archivo
                            cout << usuario << endl;
                            cout << "Usuario agregado correctamente." << endl;
                            cout << "Desea agregar otro usuario? (s/n): ";
                            cin >> continuar;
                    } while (continuar == 's' || continuar == 'S');
                }
                if (admin_opcion == 2) {
                    // por implementar
                }
                if (admin_opcion == 3) {
                    listarUsuarios();
                }
            }
            while (admin_opcion != 0);

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