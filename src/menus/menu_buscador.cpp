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
    cout << "=================================================" << endl;
    
    // Listar archivos .idx disponibles
    cout << "\nArchivos de índice invertido disponibles:" << endl;
    cout << "=================================================" << endl;
    pipe = popen("find .. -maxdepth 1 -name '*.idx' -type f 2>/dev/null", "r");
    if (pipe) {
        char buffer[256];
        int contador = 1;
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            string archivo = buffer;
            // Eliminar salto de línea
            if (!archivo.empty() && archivo[archivo.length()-1] == '\n') {
                archivo.erase(archivo.length()-1);
            }
            // Extraer solo el nombre sin la ruta
            size_t pos = archivo.find_last_of('/');
            if (pos != string::npos) {
                archivo = archivo.substr(pos + 1);
            }
            cout << contador << ". " << archivo << endl;
            contador++;
        }
        pclose(pipe);
    }
    cout << "=================================================" << endl;
    
    // Solicitar el archivo .idx a usar (con validación)
    string archivoIdx;
    string rutaIdx;
    bool archivoValido = false;
    
    while (!archivoValido) {
        cout << "\nIngrese el nombre del archivo .idx a usar: ";
        getline(cin, archivoIdx);
        
        if (archivoIdx.empty()) {
            mostrarMensajeError("Debe ingresar el nombre del archivo");
            continue;
        }
        
        if (archivoIdx.find(".idx") == string::npos) {
            archivoIdx += ".idx";
        }
        
        // Verificar que el archivo existe
        rutaIdx = "../" + archivoIdx;
        ifstream testFile(rutaIdx);
        if (!testFile.is_open()) {
            mostrarMensajeError("Ese archivo no existe. Intente nuevamente.");
        } else {
            testFile.close();
            archivoValido = true;
        }
    }
    
    cout << "\n[INFO] Iniciando sistema de búsqueda..." << endl;
    
    // 1. Lanzar MOTOR_BUSQUEDA en background
    cout << "[1/3] Iniciando MOTOR_BUSQUEDA..." << endl;
    pid_t pidMotor = fork();
    
    if (pidMotor < 0) {
        mostrarMensajeError("Error al crear proceso MOTOR_BUSQUEDA");
        return;
    }
    
    if (pidMotor == 0) {
        // Proceso hijo - MOTOR_BUSQUEDA
        setenv("TOPK", "10", 1);
        
        execl("../BUSCADOR/motor_busqueda/build/motor_busqueda",
              "motor_busqueda",
              rutaIdx.c_str(),
              nullptr);
        
        cerr << "[ERROR] No se pudo ejecutar MOTOR_BUSQUEDA" << endl;
        exit(1);
    }
    
    // Dar tiempo a que el motor inicie
    sleep(1);
    cout << "[INFO] MOTOR_BUSQUEDA iniciado (PID: " << pidMotor << ")" << endl;
    
    // 2. Lanzar CACHE en background
    cout << "[2/3] Iniciando CACHE..." << endl;
    pid_t pidCache = fork();
    
    if (pidCache < 0) {
        mostrarMensajeError("Error al crear proceso CACHE");
        kill(pidMotor, SIGTERM);
        return;
    }
    
    if (pidCache == 0) {
        // Proceso hijo - CACHE
        setenv("CACHE_SIZE", "100", 1);
        setenv("TOPK", "10", 1);
        
        execl("../BUSCADOR/cache/build/cache",
              "cache",
              nullptr);
        
        cerr << "[ERROR] No se pudo ejecutar CACHE" << endl;
        exit(1);
    }
    
    // Dar tiempo a que cache inicie
    sleep(1);
    cout << "[INFO] CACHE iniciado (PID: " << pidCache << ")" << endl;
    
    // 3. Lanzar el programa BUSCADOR
    cout << "[3/3] Iniciando BUSCADOR..." << endl;
    
    pid_t pid = fork();
    
    if (pid < 0) {
        mostrarMensajeError("Error al crear proceso hijo para BUSCADOR");
        return;
    }
    
    if (pid == 0) {
        // Proceso hijo - ejecutar buscador pasándole el archivo .idx
        cout << "[INFO] Proceso BUSCADOR, PID: " << getpid() << endl;
        
        execl("../BUSCADOR/buscador/build/buscador", 
              "buscador",
              archivoIdx.c_str(),
              nullptr);
        
        // Si execl falla
        cerr << "[ERROR] No se pudo ejecutar el BUSCADOR" << endl;
        cerr << "[ERROR] Asegúrese de que el ejecutable existe en: ../BUSCADOR/buscador/build/buscador" << endl;
        exit(1);
    } else {
        // Proceso padre - esperar a que termine el buscador
        cout << "[INFO] Esperando al BUSCADOR (PID: " << pid << ")..." << endl;
        
        int status;
        waitpid(pid, &status, 0);
        
        cout << "\n[INFO] BUSCADOR finalizado" << endl;
        
        // Terminar procesos de CACHE y MOTOR
        cout << "[INFO] Deteniendo servicios..." << endl;
        kill(pidCache, SIGTERM);
        kill(pidMotor, SIGTERM);
        
        // Esperar a que terminen
        waitpid(pidCache, nullptr, 0);
        waitpid(pidMotor, nullptr, 0);
        
        cout << "[INFO] Sistema de búsqueda detenido" << endl;
        
        if (WIFEXITED(status)) {
            int exitCode = WEXITSTATUS(status);
            if (exitCode == 0) {
                cout << "[OK] Búsqueda completada exitosamente" << endl;
            }
        }
    }
}
