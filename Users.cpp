//
// Created by Francisco Contreras on 21-08-25.
//

#include "Users.h"
#include <vector>
#include <iostream>
using namespace std;

void agregarUser(vector<User>& lista) {
    User userlc;
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Limpiar buffer antes de getline
    cout << "Ingrese nombre: ";
    getline(cin, userlc.nombre);

    cout << "Ingrese ID: ";
    while (!(cin >> userlc.id)) {
        cout << "ID inválido. Intente de nuevo: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    cout << "Ingrese tipo de perfil (1 = admin, 0 = general): ";
    while (!(cin >> userlc.perfil) || (userlc.perfil != 0 && userlc.perfil != 1)) {
        cout << "Perfil inválido. Ingrese 1 para admin o 0 para general: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Limpiar buffer

    lista.push_back(userlc);
}
void listarUsers(const vector<User>& lista) {
    for (int i = 0; i < lista.size(); i++) {
        cout << lista[i].id;
        cout << " ";
        cout << lista[i].nombre;
        cout << " ";
        cout << lista[i].perfil;
        cout << endl;

    }
}
