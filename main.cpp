#include <iostream>
#include <limits>
#include <vector>
#include <string>
#include "Users.h"
using namespace std;

void agregarUser(vector<User>& lista);
void listarUsers(const vector<User>& lista);

int main() {
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