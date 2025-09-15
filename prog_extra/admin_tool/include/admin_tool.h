// prog_extra/admin_tool/include/admin_tool.h
#ifndef ADMIN_TOOL_H
#define ADMIN_TOOL_H

#include <string>
#include <vector>

// Estructuras (podemos reutilizar las del sistema principal si es necesario)
struct Usuario {
    int id;
    std::string nombre;
    std::string username;
    std::string password;
    std::string perfil;
};

struct Perfil {
    std::string nombre;
    std::vector<int> permisos;
};

// Declaraciones de funciones
void print_pid();
bool cargarDatosDesdeArchivos();
void mostrarMenuAdminExterno();
void listarTodosUsuarios();
void listarTodosPerfiles();
void mostrarEstadisticas();
int ejecutarAdminTool();

#endif