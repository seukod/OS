#ifndef MULTI_MATRIX_H
#define MULTI_MATRIX_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unistd.h>  // Para getpid() y getcwd()
#include <iomanip>   // Para formateo de decimales

std::vector<std::vector<double>> readMatrixFromFile(const std::string& path, char separator);
bool validateMatrix(const std::vector<std::vector<double>>& matrix);
std::vector<std::vector<double>> multiplyMatrices(const std::vector<std::vector<double>>& A, const std::vector<std::vector<double>>& B);
void printMatrix(const std::vector<std::vector<double>>& matrix);

#endif // MULTI_MATRIX_H
