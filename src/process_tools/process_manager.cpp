//
// Created by Francisco Contreras on 13-09-25.
//

#include "../../include/process_tools/process_manager.h"
#include "../../include/interfaz.h"
#include "../../include/users_auth.h"
#include <fstream>


pid_t obtenerPID() {
    return getpid();
}

bool ejecutarProcesoExterno(const string& varEntorno, const string& arg1, const string& arg2) {
    cout << "Iniciando proceso externo..." << endl;
    cout << "Proceso padre, PID: " << obtenerPID() << endl;

    // Leer la variable de entorno correspondiente
    string executablePathStr = leerVariableEnv(varEntorno, ".env");

    if (executablePathStr.empty()) {
        cout << "Error: Variable " << varEntorno << " no encontrada" << endl;
        return false;
    }

    const char* executablePath = executablePathStr.c_str();
    cout << "debug: " << executablePath << endl;

    // Verificar que el archivo existe
    if (access(executablePath, F_OK) != 0) {
        cout << "Error: El archivo no existe: " << executablePath << endl;
        return false;
    }

    // Crea proceso hijo
    pid_t fpid = fork();
    if (fpid == 0) {
        // Dentro del Proceso hijo
        cout << "proceso hijo, PID: " << obtenerPID() << endl;
        pausarPantalla();
        limpiarPantalla();
        // Ejecutar según los argumentos proporcionados
        if (arg1.empty() && arg2.empty()) {
            // Solo ejecutable (caso admin_usuarios)
            execl(executablePath, "proceso_externo", NULL);
        } else if (!arg1.empty() && !arg2.empty()) {
            // Ejecutable con dos argumentos (caso multiplicador)
            execl(executablePath, "proceso_externo", arg1.c_str(), arg2.c_str(), NULL);
        } else {
            // Un argumento
            execl(executablePath, "proceso_externo", arg1.c_str(), NULL);
        }

        // Si llegamos aquí, execl falló
        cout << "Error al ejecutar " << executablePath << endl;
        exit(1);
    }
    else if (fpid > 0) {
        //Proceso padre
        cout << "Proceso padre esperando al hijo..." << endl;
        int status;
        wait(&status);

        // Verificar si el hijo terminó correctamente
        return WIFEXITED(status) && WEXITSTATUS(status) == 0;
    }
    else {
        // Error en fork()
        cout << "Error al crear proceso hijo" << endl;
        return false;
    }
}

bool ejecutarAdminUsuarios() {
    cout << "Ejecutando Administrador de Usuarios..." << endl;
    bool exito = ejecutarProcesoExterno("ADMIN_SYS");
    if (exito) {
        mostrarMensajeExito("Administrador de usuarios ejecutado correctamente.");
    } else {
        mostrarMensajeError("Error en la ejecución del administrador de usuarios.");
    }
    return exito;
}
