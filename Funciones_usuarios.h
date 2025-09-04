#ifndef OS_USERSFUNC_H
#define OS_USERSFUNC_H
#include "crud.h"
#include <vector>
#include <string>
using namespace std;

void trim(std::string &s);

void ingresarUsuario();
void eliminarUsuario(std::vector<Usuario>& usuarios);
void listarUsuarios(const vector<Usuario>& usuarios);
void ingresarPerfil();
void eliminarPerfil();
void listarPerfiles();


#endif
