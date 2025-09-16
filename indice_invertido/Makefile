CXX = g++
CXXFLAGS = -std=c++17 -Wall
TARGET = indice_invertido
SRC = src/indice_invertido.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

.PHONY: all clean