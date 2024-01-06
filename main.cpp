#include <iostream>
#include "Class/Simulacia.h"

int main() {
    Simulacia* simulacia;
    int pocetLesov, pocetLuk, pocetVod, pocetSkal, sizeX, sizeY;
    std::cout << "Zadaj velkost X: " << std::endl;
    std::cin >> sizeX;
    std::cout << "Zadaj velkost Y: " << std::endl;
    std::cin >> sizeY;
    simulacia = new Simulacia(sizeX, sizeY);
    std::cout << "Zadaj pravdepodobnost lesu: " << std::endl;
    std::cin >> pocetLesov;
    std::cout << "Zadaj pravdepodobnost luka: " << std::endl;
    std::cin >> pocetLuk;
    std::cout << "Zadaj pravdepodobnost vod: " << std::endl;
    std::cin >> pocetVod;
    std::cout << "Zadaj pocet Skal: " << std::endl;
    std::cin >> pocetSkal;

    if (simulacia->init(pocetLesov, pocetLuk, pocetVod, pocetSkal) != true) {
        std::cout << "Chyba pri inicializacii" << std::endl;
        return 1;
    }
    return 0;
}
