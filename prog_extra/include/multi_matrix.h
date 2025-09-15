#ifndef MULTI_MATRIX_H
#define MULTI_MATRIX_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unistd.h>  // Para getpid() y getcwd()
#include <iomanip>   // Para formateo de decimales

using namespace std;

vector<vector<double>> readMatrixFromFile(const string& path, char separator);
bool validateMatrix(const vector<vector<double>>& matrix);
vector<vector<double>> multiplyMatrices(const vector<vector<double>>& A, const vector<vector<double>>& B);
void printMatrix(const vector<vector<double>>& matrix);

#endif // MULTI_MATRIX_H
