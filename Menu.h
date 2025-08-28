#ifndef MENU_H
#define MENU_H

#include <string>
using namespace std;

struct Usuario {
    int id;
    string nombre;
    string username;
    string password;
    string perfil;
};

void menu(const Usuario& user);

#endif
