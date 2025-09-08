# Makefile para el programa de multiplicación de matrices

# Variables del compilador
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2
INCLUDES = -Iinclude

# Directorios
SRCDIR = source
INCDIR = include
DATADIR = data

# Archivos fuente
SOURCES = $(SRCDIR)/multi_matrix.cpp
HEADERS = $(INCDIR)/multi_matrix.h

# Ejecutable objetivo
TARGET = multi

# Regla principal
all: $(TARGET)

# Compilar el ejecutable
$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SOURCES) -o $(TARGET)

# Limpiar archivos compilados
clean:
	rm -f $(TARGET)

# Ejecutar con archivos de prueba usando coma como separador
test: $(TARGET)
	./$(TARGET) $(DATADIR)/A.txt $(DATADIR)/B.txt ","

# Ejecutar con archivos de prueba usando espacio como separador
test-space: $(TARGET)
	./$(TARGET) $(DATADIR)/A.txt $(DATADIR)/B.txt " "

# Ejecutar con archivos de prueba usando punto y coma como separador
test-semicolon: $(TARGET)
	./$(TARGET) $(DATADIR)/A.txt $(DATADIR)/B.txt ";"

# Mostrar ayuda de uso
help:
	@echo "Uso del Makefile:"
	@echo "  make          - Compila el programa"
	@echo "  make test     - Ejecuta con archivos de prueba (separador: coma)"
	@echo "  make clean    - Elimina archivos compilados"
	@echo "  make help     - Muestra esta ayuda"
	@echo ""
	@echo "Uso del programa:"
	@echo "  ./multi \"ruta_del_txt_A\" \"ruta_del_txt_B\" \"separador\""
	@echo ""
	@echo "Ejemplo:"
	@echo "  ./multi data/A.txt data/B.txt \",\""

# Validaciones incluidas en el programa:
# - Verificar que los archivos existan y se puedan abrir
# - Validar que las matrices sean cuadradas
# - Verificar que todos los elementos sean números enteros válidos
# - Comprobar que ambas matrices tengan el mismo tamaño
# - Validar formato usando el separador especificado

.PHONY: all clean test test-space test-semicolon help
