//
// Created by Francisco Contreras on 13-09-25.
//

#ifndef OS_PROCESS_MANAGER_H
#define OS_PROCESS_MANAGER_H

#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>  // Para wait()
#include <unistd.h>
#include <cstdlib>     // Para getenv() y exit()
using namespace std;

extern const char* adminPath;
pid_t obtenerPID();
bool ejecutarProcesoExterno(const string& varEntorno, const string& arg1 = "", const string& arg2 = "");
bool ejecutarAdminUsuarios();
bool ejecutarMultiplicadorMatrices(const string& archivo1, const string& archivo2);
bool ejecutarMultiplicadorDirecto();

#endif //OS_PROCESS_MANAGER_H