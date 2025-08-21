#include <iostream>

int main() {
    int opcion = 0;

    while (true) {
        std::cout << "\n===== MENU =====" << std::endl;
        std::cout << "1. Opcion 1" << std::endl;
        std::cout << "2. Opcion 2" << std::endl;
        std::cout << "3. Opcion 3" << std::endl;
        std::cout << "0. Salir" << std::endl;
        std::cout << "Elige una opcion: ";
        std::cin >> opcion;

        switch (opcion) {
            case 1:
                std::cout << "Elegiste la opcion 1" << std::endl;
                break;
            case 2:
                std::cout << "Elegiste la opcion 2" << std::endl;
                break;
            case 3:
                std::cout << "Elegiste la opcion 3" << std::endl;
                break;
            case 0:
                std::cout << "Saliendo del programa..." << std::endl;
                return 0;
            default:
                std::cout << "Opcion invalida, intenta de nuevo." << std::endl;
        }
    }
}
