#include "../include/multi_matrix.h"
#include <iomanip>

// Función para leer matriz desde archivo
vector<vector<double>> readMatrixFromFile(const string& path, char separator) {
    ifstream file(path);
    vector<vector<double>> matrix;

    if (!file.is_open()) {
        throw runtime_error("No se pudo abrir el archivo: " + path);
    }

    string line;
    while (getline(file, line)) {
        // Ignorar líneas vacías
        if (line.empty()) continue;

        vector<double> row;
        stringstream ss(line);
        string cell;
        while (getline(ss, cell, separator)) {
            try {
                row.push_back(stod(cell));
            } catch (...) {
                throw runtime_error("Elemento inválido en el archivo: " + cell);
            }
        }
        if (!row.empty()) {
            matrix.push_back(row);
        }
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
vector<vector<double>> multiplyMatrices(const vector<vector<double>>& A,
                                        const vector<vector<double>>& B) {
    size_t n = A.size();
    vector<vector<double>> result(n, vector<double>(n, 0.0));

    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j)
            for (size_t k = 0; k < n; ++k)
                result[i][j] += A[i][k] * B[k][j];

    return result;
}

// Función para imprimir matriz
void printMatrix(const vector<vector<double>>& matrix) {
    cout << fixed << setprecision(2);
    for (const auto& row : matrix) {
        for (size_t j = 0; j < row.size(); ++j) {
            // Si el número es entero, mostrarlo sin decimales
            if (row[j] == static_cast<int>(row[j])) {
                cout << static_cast<int>(row[j]);
            } else {
                cout << row[j];
            }
            if (j != row.size() - 1) cout << " ";
        }
        cout << "\n";
    }
}

// Función main independiente
int main(int argc, char* argv[]) {
    // Mostrar PID del proceso multiplicador
    cout << "Proceso multiplicador iniciado, PID: " << getpid() << endl;

    // TITULO DE LA APLICACION
    cout << "========================================" << endl;
    cout << "    MULTIPLICADOR DE MATRICES NxN" << endl;
    cout << "========================================" << endl;

    // Verificar que se recibieron los argumentos correctos
    if (argc != 3) {
        cerr << "Error: Se esperan exactamente 2 argumentos (paths de matrices)" << endl;
        cerr << "Uso: " << argv[0] << " <ruta_matriz_A> <ruta_matriz_B>" << endl;
        return 1;
    }

    // Usar los argumentos recibidos del proceso padre
    string pathA = argv[1];
    string pathB = argv[2];
    char separator = ',';  // Usar coma como separador (las matrices están en formato CSV)

    cout << "Archivo matriz A: " << pathA << endl;
    cout << "Archivo matriz B: " << pathB << endl;
    cout << "========================================" << endl;

    try {
        auto matrixA = readMatrixFromFile(pathA, separator);
        auto matrixB = readMatrixFromFile(pathB, separator);

        if (matrixA.size() != matrixB.size()) {
            throw runtime_error("Las matrices no tienen el mismo tamaño.");
        }

        cout << "Multiplicando matrices " << matrixA.size() << "x" << matrixA.size() << "..." << endl;
        auto result = multiplyMatrices(matrixA, matrixB);

        cout << "\nResultado de la multiplicación:" << endl;
        printMatrix(result);
        cout << "========================================" << endl;

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
