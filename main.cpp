#include <iostream>
#include "Class/Simulacia.h"

int main() {
    Simulacia* simulacia;
    int pocetLesov, pocetLuk, pocetVod, pocetSkal, sizeX, sizeY;
    char userInput;
    std::cout << "Fire simluation!" << std::endl;

    std::cout << "Press C to create a new map based on probabilities and size dimensions." << std::endl;
    std::cout << "Press L to load a map from a file." << std::endl;
    std::cout << "Press Q to quit." << std::endl;
    std::cin >> userInput;
    switch (userInput) {
        case 'C':
            std::cout << "Zadaj velkost X: " << std::endl;
            std::cin >> sizeX;
            std::cout << "Zadaj velkost Y: " << std::endl;
            std::cin >> sizeY;
            simulacia = new Simulacia(sizeX, sizeY);
            std::cout << "Zadaj pravdepodobnost lesu: " << std::endl;
            std::cin >> pocetLesov;
            std::cout << "Zadaj pravdepodobnost luky: " << std::endl;
            std::cin >> pocetLuk;
            std::cout << "Zadaj pravdepodobnost vody: " << std::endl;
            std::cin >> pocetVod;
            std::cout << "Zadaj pravdepodobnost Skal: " << std::endl;
            std::cin >> pocetSkal;
            if (simulacia->init(pocetLesov, pocetLuk, pocetVod, pocetSkal) != true) {
                std::cout << "Chyba pri inicializacii" << std::endl;
                return 1;
            }
            break;
        case 'L': {
            std::cout << "Enter a filename to load the simulation : ";
            std::string fileName;
            std::cin >> fileName;
            simulacia = new Simulacia(0, 0);
            if (!simulacia->loadFile(fileName.c_str())) {
                std::cout << "Chyba pri nacitani suboru" << std::endl;
                return 1;
            }
            break;
        }
        case 'Q':
            return 0;
            break;
        default:
            std::cout << "Neznama volba" << std::endl;
            return 1;


    }

    std::thread mainThread(&Simulacia::getUserInput, simulacia);
    std::thread simulationThread(&Simulacia::runMutexLogic, simulacia);

    mainThread.join();
    simulationThread.join();

    delete simulacia;
    return 0;
}


