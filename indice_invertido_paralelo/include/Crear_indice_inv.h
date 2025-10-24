//
// Created by Francisco Contreras on 24-10-25.
//

#ifndef OS_CREAR_INDICE_INV_H
#define OS_CREAR_INDICE_INV_H

#include <filesystem>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unistd.h>  // Para getpid()


std::vector<std::string> escanearLibros(const std::string& rutaLibros);  // Agregar parámetro
void actualizarMapaLibros(const std::vector<std::string>& libros);
//void crearIndiceInvertidoParalelo();


#endif //OS_CREAR_INDICE_INV_H

