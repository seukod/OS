#include "../../include/menus/menu_juego.h"
#include "../../include/interfaz.h"
#include "../../include/utils/input_utils.h"
#include <iostream>
#include <cstdlib>
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif
#endif

using namespace std;

void ejecutarMenuJuego() {
    limpiarPantalla();
    cout << "=================================================" << endl;
    cout << "           BATTLE CITY - MULTIPLAYER             " << endl;
    cout << "=================================================" << endl;
    cout << endl;
    cout << "Iniciando juego Battle City..." << endl;
    cout << "Este es un juego multijugador (hasta 4 jugadores)" << endl;
    cout << "usando arquitectura P2P (Peer-to-Peer)." << endl;
    cout << endl;
    cout << "Instrucciones:" << endl;
    cout << "- Un jugador debe HOSTEAR la partida" << endl;
    cout << "- Los otros 3 pueden UNIRSE ingresando la IP del host" << endl;
    cout << "- El host puede jugar mientras otros se conectan" << endl;
    cout << endl;
    cout << "Controles en el juego:" << endl;
    cout << "- Flechas: Mover tanque" << endl;
    cout << "- Espacio: Disparar" << endl;
    cout << "- R: Marcar como listo (en lobby)" << endl;
    cout << endl;
    cout << "=================================================" << endl;
    cout << "Presiona ENTER para continuar...";
    cin.ignore();
    cin.get();

    // Obtener ruta del ejecutable Python
    string pythonPath;
    string gamePath;

    #ifdef _WIN32
        // En Windows
        // Intentar encontrar Python en el PATH o usar una ruta común
        pythonPath = "python";  // Intenta usar python del PATH
        
        // Obtener la ruta del juego (asumiendo que está en Juego/game_multiplayer.py)
        char buffer[MAX_PATH];
        GetModuleFileNameA(NULL, buffer, MAX_PATH);
        string exePath(buffer);
        size_t pos = exePath.find_last_of("\\/");
        string baseDir = exePath.substr(0, pos);
        gamePath = baseDir + "\\..\\Juego\\game_multiplayer.py";
        
        // Comando para ejecutar el juego
        string command = pythonPath + " \"" + gamePath + "\"";
        
        cout << "[INFO] Ejecutando: " << command << endl;
        cout << endl;
        
        // Ejecutar el juego de Python
        int result = system(command.c_str());
        
        if (result != 0) {
            cerr << "[ERROR] No se pudo ejecutar el juego." << endl;
            cerr << "[AYUDA] Asegúrate de tener Python instalado y en el PATH." << endl;
            cerr << "[AYUDA] También puedes ejecutar manualmente:" << endl;
            cerr << "        " << command << endl;
        }
        
    #else
        // En Linux/Mac
        // Intentar usar el entorno virtual primero
        pythonPath = ".venv/bin/python";
        
        // Obtener directorio del ejecutable
        char buffer[1024];
        #ifdef __APPLE__
            // En macOS, usar _NSGetExecutablePath
            uint32_t size = sizeof(buffer);
            if (_NSGetExecutablePath(buffer, &size) == 0) {
                string exePath(buffer);
                size_t pos = exePath.find_last_of("/");
                string baseDir = exePath.substr(0, pos);
                gamePath = baseDir + "/../Juego/game_multiplayer.py";
                pythonPath = baseDir + "/../.venv/bin/python";
            } else {
                gamePath = "../Juego/game_multiplayer.py";
                pythonPath = ".venv/bin/python";
            }
        #else
            // En Linux
            ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer)-1);
            if (len != -1) {
                buffer[len] = '\0';
                string exePath(buffer);
                size_t pos = exePath.find_last_of("/");
                string baseDir = exePath.substr(0, pos);
                gamePath = baseDir + "/../Juego/game_multiplayer.py";
                pythonPath = baseDir + "/../.venv/bin/python";
            } else {
                gamePath = "../Juego/game_multiplayer.py";
                pythonPath = ".venv/bin/python";
            }
        #endif
        
        // Fork para ejecutar Python
        pid_t pid = fork();
        
        if (pid == 0) {
            // Proceso hijo: ejecutar Python
            execlp(pythonPath.c_str(), pythonPath.c_str(), gamePath.c_str(), nullptr);
            // Si execlp falla
            cerr << "[ERROR] No se pudo ejecutar Python" << endl;
            exit(1);
        } else if (pid > 0) {
            // Proceso padre: esperar a que termine
            int status;
            waitpid(pid, &status, 0);
            
            if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
                cerr << "[ERROR] El juego terminó con errores." << endl;
            }
        } else {
            cerr << "[ERROR] No se pudo crear el proceso." << endl;
        }
    #endif
    
    cout << endl;
    cout << "El juego ha terminado." << endl;
    pausarPantalla();
}
