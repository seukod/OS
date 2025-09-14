#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstring>

using namespace std;

// Función para leer matriz desde archivo
vector<vector<int>> readMatrixFromFile(const string& path, char separator) {
    ifstream file(path);
    vector<vector<int>> matrix;

    if (!file.is_open()) {
        throw runtime_error("No se pudo abrir el archivo: " + path);
    }

    string line;
    while (getline(file, line)) {
        // Validar líneas vacías
        if (line.empty()) continue;
        
        // Validar espacios en blanco
        if (line.find_first_not_of(" \t\r\n") == string::npos) continue;
        
        vector<int> row;
        stringstream ss(line);
        string cell;
        while (getline(ss, cell, separator)) {
            // Validar celdas vacías
            if (cell.empty()) {
                throw runtime_error("Celda vacía encontrada en línea: " + line);
            }
            
            try {
                row.push_back(stoi(cell));
            } catch (...) {
                throw runtime_error("Elemento inválido en el archivo: " + cell);
            }
        }
        
        // Validar filas vacías
        if (row.empty()) {
            throw runtime_error("Fila vacía encontrada");
        }
        
        matrix.push_back(row);
    }

    // Validar matriz vacía
    if (matrix.empty()) {
        throw runtime_error("El archivo está vacío o no contiene datos válidos");
    }

    // Validar que la matriz sea cuadrada
    size_t n = matrix.size();
    for (const auto& row : matrix) {
        if (row.size() != n) {
            throw runtime_error("La matriz no es cuadrada o está mal formada.");
        }
    }

    return matrix;
}

// Función para multiplicar dos matrices NxN
vector<vector<int>> multiplyMatrices(const vector<vector<int>>& A,
                                     const vector<vector<int>>& B) {
    size_t n = A.size();
    vector<vector<int>> result(n, vector<int>(n, 0));

    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j)
            for (size_t k = 0; k < n; ++k)
                result[i][j] += A[i][k] * B[k][j];

    return result;
}

// Función para imprimir matriz
void printMatrix(const vector<vector<int>>& matrix) {
    for (const auto& row : matrix) {
        for (size_t j = 0; j < row.size(); ++j) {
            cout << row[j];
            if (j != row.size() - 1) cout << " ";
        }
        cout << "\n";
    }
}

// Función main independiente
int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "Uso: " << argv[0] << " <ruta_a_A.txt> <ruta_a_B.txt> <separador>\n";
        return 1;
    }

    // Validar argumentos no nulos
    if (!argv[1] || !argv[2] || !argv[3]) {
        cerr << "Error: Argumentos nulos\n";
        return 1;
    }

    // Validar longitud del separador
    if (strlen(argv[3]) != 1) {
        cerr << "Error: El separador debe ser un solo carácter\n";
        return 1;
    }

    // Validar rutas no vacías
    string pathA = argv[1];
    string pathB = argv[2];
    char separator = argv[3][0];
    
    if (pathA.empty() || pathB.empty()) {
        cerr << "Error: Las rutas de archivo no pueden estar vacías\n";
        return 1;
    }

    try {
        auto matrixA = readMatrixFromFile(pathA, separator);
        auto matrixB = readMatrixFromFile(pathB, separator);

        if (matrixA.size() != matrixB.size()) {
            throw runtime_error("Las matrices no tienen el mismo tamaño.");
        }

        auto result = multiplyMatrices(matrixA, matrixB);

        cout << "Resultado de la multiplicación:\n";
        printMatrix(result);
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
