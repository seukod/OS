//
// Created by Francisco Contreras on 13-09-25.
//

#include "../../include/process_tools/process_manager.h"
#include "../../include/interfaz.h"
#include "../../include/users_auth.h"
#include <fstream>
#include <cstdlib>  // Para getcwd()


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

bool ejecutarMultiplicadorMatrices(const string& archivo1, const string& archivo2) {
    cout << "Ejecutando Multiplicador de Matrices..." << endl;
    bool exito = ejecutarProcesoExterno("MULTI_M", archivo1, archivo2);
    return exito;
}

bool ejecutarMultiplicadorDirecto() {
    limpiarPantalla();
    cout << "Ejecutando Multiplicador de Matrices..." << endl;

    // Crear interfaz previa como requiere el punto 4a)
    cout << "\n========================================" << endl;
    cout << "    CONFIGURACIÓN DE MATRICES" << endl;
    cout << "========================================" << endl;

    // i) Ingresar el N de la matriz
    int n;
    cout << "Ingrese el tamaño N de las matrices (NxN): ";
    cin >> n;
    cin.ignore(); // Limpiar buffer

    // ii) Ingresar los paths de las dos matrices
    string archivo1, archivo2;
    cout << "Ingrese el path del primer archivo de matriz: ";
    getline(cin, archivo1);
    cout << "Ingrese el path del segundo archivo de matriz: ";
    getline(cin, archivo2);

    // Mostrar resumen de configuración
    cout << "\n--- RESUMEN DE CONFIGURACIÓN ---" << endl;
    cout << "Tamaño de matrices: " << n << "x" << n << endl;
    cout << "Archivo matriz A: " << archivo1 << endl;
    cout << "Archivo matriz B: " << archivo2 << endl;
    cout << "================================" << endl;

    // iii) Lanzar la ejecución o volver
    cout << "\n1. Lanzar ejecución" << endl;
    cout << "2. Volver al menú principal" << endl;
    cout << "Seleccione una opción: ";

    int opcion;
    cin >> opcion;
    cin.ignore(); // Limpiar buffer

    if (opcion == 1) {
        limpiarPantalla();
        // Convertir rutas relativas a absolutas antes de validar
        string archivoAbsoluto1 = archivo1;
        string archivoAbsoluto2 = archivo2;

        // Si la ruta no empieza con '/', es relativa, convertirla a absoluta
        if (archivo1[0] != '/') {
            char* cwd = getcwd(nullptr, 0);
            archivoAbsoluto1 = string(cwd) + "/../" + archivo1;
            free(cwd);
        }
        if (archivo2[0] != '/') {
            char* cwd = getcwd(nullptr, 0);
            archivoAbsoluto2 = string(cwd) + "/../" + archivo2;
            free(cwd);
        }

        // Validar existencia de archivos con rutas absolutas
        ifstream file1(archivoAbsoluto1);
        if (!file1.good()) {
            cout << "Error: No se encuentra el primer archivo: " << archivoAbsoluto1 << endl;
            return false;
        }
        file1.close();

        ifstream file2(archivoAbsoluto2);
        if (!file2.good()) {
            cout << "Error: No se encuentra el segundo archivo: " << archivoAbsoluto2 << endl;
            return false;
        }
        file2.close();

        cout << "Archivos validados correctamente:" << endl;
        cout << "Archivo 1: " << archivoAbsoluto1 << endl;
        cout << "Archivo 2: " << archivoAbsoluto2 << endl;
        cout << "Ejecutando multiplicador..." << endl;
        pausarPantalla();
        limpiarPantalla();
        // Ejecutar el proceso externo PASANDO las rutas absolutas como argumentos
        bool exito = ejecutarMultiplicadorMatrices(archivoAbsoluto1, archivoAbsoluto2);
        if (exito) {
            mostrarMensajeExito("Multiplicación completada exitosamente.");
        } else {
            mostrarMensajeError("Error en la ejecución del multiplicador.");
        }
        return exito;
    } else {
        cout << "Volviendo al menú principal..." << endl;
        return true;
    }
}
