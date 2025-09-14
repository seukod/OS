#include "../include/multi_matrix.h"
#include <iomanip>

// Función para leer matriz desde archivo
std::vector<std::vector<double>> readMatrixFromFile(const std::string& path, char separator) {
    std::ifstream file(path);
    std::vector<std::vector<double>> matrix;

    if (!file.is_open()) {
        throw std::runtime_error("No se pudo abrir el archivo: " + path);
    }

    std::string line;
    while (std::getline(file, line)) {
        // Ignorar líneas vacías
        if (line.empty()) continue;

        std::vector<double> row;
        std::stringstream ss(line);
        std::string cell;
        while (std::getline(ss, cell, separator)) {
            try {
                row.push_back(std::stod(cell));
            } catch (...) {
                throw std::runtime_error("Elemento inválido en el archivo: " + cell);
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
            throw std::runtime_error("La matriz no es cuadrada o está mal formada.");
        }
    }

    return matrix;
}

// Función para multiplicar dos matrices NxN
std::vector<std::vector<double>> multiplyMatrices(const std::vector<std::vector<double>>& A,
                                                  const std::vector<std::vector<double>>& B) {
    size_t n = A.size();
    std::vector<std::vector<double>> result(n, std::vector<double>(n, 0.0));

    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j)
            for (size_t k = 0; k < n; ++k)
                result[i][j] += A[i][k] * B[k][j];

    return result;
}

// Función para imprimir matriz
void printMatrix(const std::vector<std::vector<double>>& matrix) {
    std::cout << std::fixed << std::setprecision(2);
    for (const auto& row : matrix) {
        for (size_t j = 0; j < row.size(); ++j) {
            // Si el número es entero, mostrarlo sin decimales
            if (row[j] == static_cast<int>(row[j])) {
                std::cout << static_cast<int>(row[j]);
            } else {
                std::cout << row[j];
            }
            if (j != row.size() - 1) std::cout << " ";
        }
        std::cout << "\n";
    }
}

// Función main independiente
int main(int argc, char* argv[]) {
    // Mostrar PID del proceso multiplicador
    std::cout << "Proceso multiplicador iniciado, PID: " << getpid() << std::endl;

    // TITULO DE LA APLICACION
    std::cout << "========================================" << std::endl;
    std::cout << "    MULTIPLICADOR DE MATRICES NxN" << std::endl;
    std::cout << "========================================" << std::endl;

    // Verificar que se recibieron los argumentos correctos
    if (argc != 3) {
        std::cerr << "Error: Se esperan exactamente 2 argumentos (paths de matrices)" << std::endl;
        std::cerr << "Uso: " << argv[0] << " <ruta_matriz_A> <ruta_matriz_B>" << std::endl;
        return 1;
    }

    // Usar los argumentos recibidos del proceso padre
    std::string pathA = argv[1];
    std::string pathB = argv[2];
    char separator = ',';  // Usar coma como separador (las matrices están en formato CSV)

    std::cout << "Archivo matriz A: " << pathA << std::endl;
    std::cout << "Archivo matriz B: " << pathB << std::endl;
    std::cout << "========================================" << std::endl;

    try {
        auto matrixA = readMatrixFromFile(pathA, separator);
        auto matrixB = readMatrixFromFile(pathB, separator);

        if (matrixA.size() != matrixB.size()) {
            throw std::runtime_error("Las matrices no tienen el mismo tamaño.");
        }

        std::cout << "Multiplicando matrices " << matrixA.size() << "x" << matrixA.size() << "..." << std::endl;
        auto result = multiplyMatrices(matrixA, matrixB);

        std::cout << "\nResultado de la multiplicación:" << std::endl;
        printMatrix(result);
        std::cout << "========================================" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
