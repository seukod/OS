#include "../../include/menus/menu_indice.h"
#include "../../include/interfaz.h"
#include "../../include/utils/input_utils.h"
#include "../../include/users_auth.h"
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>

using namespace std;

bool validarNombreArchivo(const string& nombreArchivo) {
    if (nombreArchivo.length() < 5) {
        return false;
    }
    return nombreArchivo.substr(nombreArchivo.length() - 4) == ".idx";
}

bool validarDirectorio(const string& pathDirectorio) {
    struct stat info;
    if (stat(pathDirectorio.c_str(), &info) != 0) {
        return false; // No existe
    }
    return (info.st_mode & S_IFDIR) != 0; // Es un directorio
}

bool crearIndiceInvertido(const string& nombreArchivo, const string& pathCarpeta) {
    // Obtener la variable de entorno CREATE_INDEX usando la función del sistema
    string createIndexPath = leerVariableEnv("CREATE_INDEX", ".env");
    if (createIndexPath.empty()) {
        cout << "[ERROR] Variable de entorno CREATE_INDEX no está definida." << endl;
        return false;
    }

    cout << "\n=================================================" << endl;
    cout << "           CREANDO ÍNDICE INVERTIDO              " << endl;
    cout << "=================================================" << endl;
    cout << "Programa: " << createIndexPath << endl;
    cout << "Archivo: " << nombreArchivo << endl;
    cout << "Directorio libros: " << pathCarpeta << endl;
    cout << "=================================================" << endl;
    cout << "\nIniciando proceso..." << endl;

    // Crear proceso hijo usando fork
    pid_t pid = fork();
    
    if (pid == -1) {
        cout << "[ERROR] No se pudo crear el proceso hijo." << endl;
        return false;
    }
    
    if (pid == 0) {
        // Proceso hijo - ejecutar el programa externo
        execl(createIndexPath.c_str(), "crear_indice", nombreArchivo.c_str(), pathCarpeta.c_str(), nullptr);
        
        // Si llegamos aquí, execl falló
        cout << "[ERROR] No se pudo ejecutar el programa " << createIndexPath << endl;
        exit(1);
    } else {
        // Proceso padre - esperar a que termine el hijo
        int status;
        waitpid(pid, &status, 0);
        
        if (WIFEXITED(status)) {
            int exitCode = WEXITSTATUS(status);
            if (exitCode == 0) {
                cout << "\n¡Índice invertido creado exitosamente!" << endl;
                return true;
            } else {
                cout << "\n[ERROR] El programa terminó con código de error: " << exitCode << endl;
                return false;
            }
        } else {
            cout << "\n[ERROR] El programa no terminó normalmente." << endl;
            return false;
        }
    }
}

void ejecutarMenuIndice() {
    limpiarPantalla();
    cout << "=================================================" << endl;
    cout << "           CREAR ÍNDICE INVERTIDO                " << endl;
    cout << "=================================================" << endl;
    
    string nombreArchivo;
    string pathCarpeta;
    
    // Paso 1: Ingresar y validar nombre del archivo
    do {
        cout << "\nIngrese el nombre del archivo a crear (debe terminar en .idx): ";
        getline(cin, nombreArchivo);
        
        if (nombreArchivo.empty()) {
            cout << "[ERROR] El nombre no puede estar vacío." << endl;
        } else if (!validarNombreArchivo(nombreArchivo)) {
            cout << "[ERROR] El archivo debe tener extensión .idx" << endl;
        } else {
            break;
        }
    } while (true);
    
    // Paso 2: Ingresar y validar path de la carpeta
    do {
        cout << "\nIngrese el path de la carpeta donde están los libros: ";
        getline(cin, pathCarpeta);
        
        if (pathCarpeta.empty()) {
            cout << "[ERROR] El path no puede estar vacío." << endl;
        } else if (!validarDirectorio(pathCarpeta)) {
            cout << "[ERROR] El directorio no existe o no es válido." << endl;
        } else {
            break;
        }
    } while (true);
    
    // Paso 3: Confirmar antes de ejecutar
    cout << "\n=================================================" << endl;
    cout << "RESUMEN:" << endl;
    cout << "Archivo a crear: " << nombreArchivo << endl;
    cout << "Directorio de libros: " << pathCarpeta << endl;
    cout << "=================================================" << endl;
    
    char confirmacion;
    cout << "¿Desea proceder? (s/n): ";
    cin >> confirmacion;
    cin.ignore(); // Limpiar buffer
    
    if (confirmacion == 's' || confirmacion == 'S') {
        // Paso 4: Ejecutar programa externo
        if (crearIndiceInvertido(nombreArchivo, pathCarpeta)) {
            cout << "\nOperación completada exitosamente." << endl;
        } else {
            cout << "\nLa operación falló. Revise los mensajes de error." << endl;
        }
    } else {
        cout << "\nOperación cancelada por el usuario." << endl;
    }
    
    cout << "\nPresione Enter para continuar...";
    cin.ignore();
}
