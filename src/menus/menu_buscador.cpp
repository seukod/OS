#include "../../include/menus/menu_buscador.h"
#include "../../include/interfaz.h"
#include "../../include/process_tools/process_manager.h"
#include <iostream>
#include <fstream>
#include <cstdio>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

void ejecutarMenuBuscador() {
    limpiarPantalla();
    cout << "=================================================" << endl;
    cout << "              BUSCADOR SISTOPE                   " << endl;
    cout << "=================================================" << endl;
    cout << "OS PID: " << getpid() << endl;
    cout << "=================================================" << endl;
    
    // Verificar que exista al menos un archivo .idx
    FILE* pipe = popen("find .. -maxdepth 1 -name '*.idx' -type f 2>/dev/null | wc -l", "r");
    int numArchivos = 0;
    if (pipe) {
        fscanf(pipe, "%d", &numArchivos);
        pclose(pipe);
    }
    
    if (numArchivos == 0) {
        mostrarMensajeError("No se encontraron archivos de índice invertido (.idx)");
        cout << "Debe crear primero un índice invertido (opción 7 u 8 del menú)." << endl;
        return;
    }
    
    cout << "\n[INFO] Se encontraron " << numArchivos << " archivo(s) .idx" << endl;
    cout << "El BUSCADOR le solicitará cuál desea utilizar." << endl;
    cout << "=================================================" << endl;
    
    // Lanzar el programa BUSCADOR
    cout << "\nIniciando BUSCADOR..." << endl;
    
    pid_t pid = fork();
    
    if (pid < 0) {
        mostrarMensajeError("Error al crear proceso hijo para BUSCADOR");
        return;
    }
    
    if (pid == 0) {
        // Proceso hijo - ejecutar buscador
        cout << "[INFO] Proceso BUSCADOR, PID: " << getpid() << endl;
        
        execl("../BUSCADOR/buscador/build/buscador", 
              "buscador", 
              nullptr);
        
        // Si execl falla
        cerr << "[ERROR] No se pudo ejecutar el BUSCADOR" << endl;
        cerr << "[ERROR] Asegúrese de que el ejecutable existe en: ../BUSCADOR/buscador/build/buscador" << endl;
        exit(1);
    } else {
        // Proceso padre - esperar a que termine el buscador
        cout << "[INFO] Proceso padre esperando al BUSCADOR (PID: " << pid << ")..." << endl;
        
        int status;
        waitpid(pid, &status, 0);
        
        if (WIFEXITED(status)) {
            int exitCode = WEXITSTATUS(status);
            if (exitCode == 0) {
                cout << "\n[INFO] BUSCADOR finalizado correctamente" << endl;
            } else {
                mostrarMensajeError("BUSCADOR terminó con error (código: " + to_string(exitCode) + ")");
            }
        } else {
            mostrarMensajeError("BUSCADOR terminó anormalmente");
        }
    }
}
