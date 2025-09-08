#ifndef MULTI_MATRIX_H
#define MULTI_MATRIX_H

#include <string>
#include <vector>

std::vector<std::vector<int>> readMatrixFromFile(const std::string& path, char separator);
bool validateMatrix(const std::vector<std::vector<int>>& matrix);
std::vector<std::vector<int>> multiplyMatrices(const std::vector<std::vector<int>>& A, const std::vector<std::vector<int>>& B);
void printMatrix(const std::vector<std::vector<int>>& matrix);

#endif // MULTI_MATRIX_H
