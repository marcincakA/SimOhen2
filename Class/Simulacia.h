//
// Created by marci on 1/3/2024.
//

#ifndef SIMOHENKLIENT_SIMULACIA_H
#define SIMOHENKLIENT_SIMULACIA_H
#include "Biotop.h"
#include <random>
#include <iostream>
#include <fstream>
#include <mutex>
#include <thread>
#include <condition_variable>


enum VietorEnum {
    SEVER = 0,
    VYCHOD = 1,
    JUH = 2,
    ZAPAD = 3,
    BEZVETRIE = 4
};

class Simulacia {
private:
    int sizeX, sizeY;
    Biotop** biotop;
    VietorEnum vietor;
    unsigned int pocetSimulacii = 0;
    unsigned int zaciatokVetra = 0;
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<> dis;
    std::string zadanyZnak;
    std::mutex mutex;
    std::mutex consoleMutex;
    std::condition_variable cv, cv2;
    bool isPrinting;

public:
    Simulacia(int sizeX, int sizeY) : sizeX(sizeX), sizeY(sizeY) {
        biotop = new Biotop*[sizeX];
        for(int i = 0; i < sizeX; i++) {
            biotop[i] = new Biotop[sizeY];
        }
        this->vietor = BEZVETRIE;
        this->gen = std::mt19937(rd());
        this->dis = std::uniform_int_distribution<>(0, 100);
        this->zadanyZnak = ""; // pre mutex
        this->isPrinting = true;
    };
    ~Simulacia() {
        // Deallocate memory for the 2D array
        for (int i = 0; i < sizeX; ++i) {
            delete[] biotop[i];
        }
        delete[] biotop;
    }

    bool positionIsValid(int x, int y) {
        return x >= 0 && x < sizeX && y >= 0 && y < sizeY;
    }

    bool init(int percentoLes, int percentoLuka, int percentoSkala, int percentoVoda) {
        if (percentoVoda + percentoSkala + percentoLuka + percentoLes != 100) {
            std::cout << "Zla percentualna hodnota";
            return false;
        }
        if (percentoVoda < 0 || percentoSkala < 0 || percentoLuka < 0 || percentoLes < 0) {
            std::cout << "Zla percentualna hodnota";
            return false;
        }

        for(int i = 0; i < sizeX; i++) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, 100);
            int cislo = dis(gen);
            int cislo2 = dis(gen);
            //mapa init random hodnoty
            for(int j = 0; j < sizeY; j++) {
                biotop[i][j].setPosX(i);
                biotop[i][j].setPosY(j);
                biotop[i][j].setZhorena(false);
                if (dis(gen) <= percentoLes) {
                    biotop[i][j].setStav(LES);
                } else if (dis(gen) <= percentoLuka) {
                    biotop[i][j].setStav(LUKA);
                } else if (dis(gen) <= percentoSkala) {
                    biotop[i][j].setStav(SKALA);
                } else {
                    biotop[i][j].setStav(VODA);
                }
            }

            //vietor init
            if (cislo2 <= 90) {
                this->vietor = BEZVETRIE;
            } else {
                int smerVetra = dis(gen);
                if (smerVetra <= 25) {
                    this->vietor = SEVER;
                } else if (smerVetra <= 50) {
                    this->vietor = VYCHOD;
                } else if (smerVetra <= 75) {
                    this->vietor = JUH;
                } else {
                    this->vietor = ZAPAD;
                }
                this->zaciatokVetra = 1;
            }
        }
        return true;
    }

    //zatial takto neskor treba producent a konzument
    void print() {
        std::cout << "Smer vetra: " << this->vietor << std::endl;
        for(int i = 0; i < sizeX; i++) {
            for(int j = 0; j < sizeY; j++) {
                std::cout << biotop[i][j].getStav() << " ";
            }
            std::cout << std::endl;
        }
    }

    void setFlame(int x, int y) {
        if (positionIsValid(x, y)) {
            if (!biotop[x][y].isFlamable()) {
                std::cout << "Bunka nie je horlava" << std::endl;
                return;
            }
            biotop[x][y].setStav(POZIAR);
            biotop[x][y].setZaciatokHorenia(this->pocetSimulacii);
        } else {
            std::cout << "Zadana pozicia nie je validna" << std::endl;
        }
    }

    void windStep() {
        if(this->vietor != BEZVETRIE) {
            //vietor drzi svoj smer na tri tahy
            if (this->zaciatokVetra == this->pocetSimulacii - 3) {
                this->vietor = BEZVETRIE;
                return;
            }

            if(this->vietor == BEZVETRIE) {

                int cislo = dis(gen);
                if (cislo <= 90) {
                    this->vietor = BEZVETRIE;
                } else {
                    int smerVetra = dis(gen);
                    if (smerVetra <= 25) {
                        this->vietor = SEVER;
                    } else if (smerVetra <= 50) {
                        this->vietor = VYCHOD;
                    } else if (smerVetra <= 75) {
                        this->vietor = JUH;
                    } else {
                        this->vietor = ZAPAD;
                    }
                    this->zaciatokVetra = this->pocetSimulacii;
                }
            }


        }
    }

    void setFireArround(int x, int y) { // x je vyska y je sirka
        //metodka co zoberie stav vetra a bunku na zaklade vetra zapali ostatne bunky
        if (!positionIsValid(x, y)) {
            std::cout << "Zadana pozicia nie je validna" << std::endl;
            return;
        }
        if (!biotop[x][y].getStav() == POZIAR) {
            std::cout << "Bunka nehori" << std::endl;
            return;
        }

        //*******************BEZVETRIE*******************//
        if (this->vietor == BEZVETRIE) {
            if (positionIsValid(x - 1, y)) {
                if (biotop[x - 1][y].isFlamable()) {
                    int pravdepodobnostPoziaru = dis(gen);
                    if (pravdepodobnostPoziaru <= 20) { // 20 percentna sanca na poziar
                        setFlame(x - 1, y);
                    }
                }
            }
            if (positionIsValid(x + 1, y)) {
                if (biotop[x + 1][y].isFlamable()) {
                    int pravdepodobnostPoziaru = dis(gen);
                    if (pravdepodobnostPoziaru <= 20) { // 20 percentna sanca na poziar
                        //biotop[x + 1][y].setStav(POZIAR);
                        setFlame(x + 1, y);
                    }
                }
            }
            if (positionIsValid(x, y - 1)) {
                if (biotop[x][y - 1].isFlamable()) {
                    int pravdepodobnostPoziaru = dis(gen);
                    if (pravdepodobnostPoziaru <= 20) { // 20 percentna sanca na poziar
                        //biotop[x][y - 1].setStav(POZIAR);
                        setFlame(x, y - 1);
                    }
                }
            }
            if (positionIsValid(x, y + 1)) {
                if (biotop[x][y + 1].isFlamable()) {
                    int pravdepodobnostPoziaru = dis(gen);
                    if (pravdepodobnostPoziaru <= 20) { // 20 percentna sanca na poziar
                        //biotop[x][y + 1].setStav(POZIAR);
                        setFlame(x, y + 1);
                    }
                }
            }
        }
        /************************SEVER**********************/
        else if(this->vietor == SEVER) {
            if (positionIsValid(x - 1, y)) {
                if (biotop[x - 1][y].isFlamable()) {
                    int pravdepodobnostPoziaru = dis(gen);
                    if (pravdepodobnostPoziaru <= 90) { // 90 percentna sanca na poziar
                        //biotop[x - 1][y].setStav(POZIAR);
                        setFlame(x - 1, y);
                    }
                }
            }
            if (positionIsValid(x + 1, y)) {
                if (biotop[x + 1][y].isFlamable()) {
                    int pravdepodobnostPoziaru = dis(gen);
                    if (pravdepodobnostPoziaru <= 2) { // 2 percentna sanca na poziar
                        //biotop[x +1][y].setStav(POZIAR);
                        setFlame(x + 1, y);
                    }
                }
            }
            if (positionIsValid(x, y - 1)) {
                if (biotop[x][y - 1].isFlamable()) {
                    int pravdepodobnostPoziaru = dis(gen);
                    if (pravdepodobnostPoziaru <= 20) { // 20 percentna sanca na poziar ostava ako pri bezvetri
                        //biotop[x][y - 1].setStav(POZIAR);
                        setFlame(x, y - 1);
                    }
                }
            }
            if (positionIsValid(x, y + 1)) {
                if (biotop[x][y + 1].isFlamable()) {
                    int pravdepodobnostPoziaru = dis(gen);
                    if (pravdepodobnostPoziaru <= 20) { // 20 percentna sanca na poziar ostava ako pri bezvetri
                        //biotop[x][y + 1].setStav(POZIAR);
                        setFlame(x, y + 1);
                    }
                }
            }
        /************************VYCHOD**********************/
        } else if(this->vietor == VYCHOD) {
            if (positionIsValid(x - 1, y)) {
                if (biotop[x - 1][y].isFlamable()) {
                    int pravdepodobnostPoziaru = dis(gen);
                    if (pravdepodobnostPoziaru <= 20) { // 20 percentna sanca na poziar
                        //biotop[x - 1][y].setStav(POZIAR);
                        setFlame(x - 1, y);
                    }
                }
            }
            if (positionIsValid(x + 1, y)) {
                if (biotop[x + 1][y].isFlamable()) {
                    int pravdepodobnostPoziaru = dis(gen);
                    if (pravdepodobnostPoziaru <= 20) { // 2 percentna sanca na poziar
                        //biotop[x + 1][y].setStav(POZIAR);
                        setFlame(x + 1, y);
                    }
                }
            }
            if (positionIsValid(x, y - 1)) {
                if (biotop[x][y - 1].isFlamable()) {
                    int pravdepodobnostPoziaru = dis(gen);
                    if (pravdepodobnostPoziaru <= 2) { // 2 percentna sanca na poziar
                        //biotop[x][y - 1].setStav(POZIAR);
                        setFlame(x, y - 1);
                    }
                }
            }
            if (positionIsValid(x, y + 1)) {
                if (biotop[x][y + 1].isFlamable()) {
                    int pravdepodobnostPoziaru = dis(gen);
                    if (pravdepodobnostPoziaru <= 90) { // 90 percentna sanca na poziar
                        //biotop[x][y + 1].setStav(POZIAR);
                        setFlame(x, y + 1);
                    }
                }
            }
        /************************JUH**********************/
        } else if(this->vietor == JUH) {
            if (positionIsValid(x - 1, y)) {
                if (biotop[x - 1][y].isFlamable()) {
                    int pravdepodobnostPoziaru = dis(gen);
                    if (pravdepodobnostPoziaru <= 2) { // 2 percentna sanca na poziar
                        //biotop[x - 1][y].setStav(POZIAR);
                        setFlame(x - 1, y);
                    }
                }
            }
            if (positionIsValid(x + 1, y)) {
                if (biotop[x + 1][y].isFlamable()) {
                    int pravdepodobnostPoziaru = dis(gen);
                    if (pravdepodobnostPoziaru <= 90) { // 90 percentna sanca na poziar
                        //biotop[x + 1][y].setStav(POZIAR);
                        setFlame(x + 1, y);
                    }
                }
            }
            if (positionIsValid(x, y - 1)) {
                if (biotop[x][y - 1].isFlamable()) {
                    int pravdepodobnostPoziaru = dis(gen);
                    if (pravdepodobnostPoziaru <= 20) { // 2 percentna sanca na poziar
                        //biotop[x][y - 1].setStav(POZIAR);
                        setFlame(x, y - 1);
                    }
                }
            }
            if (positionIsValid(x, y + 1)) {
                if (biotop[x][y + 1].isFlamable()) {
                    int pravdepodobnostPoziaru = dis(gen);
                    if (pravdepodobnostPoziaru <= 20) { // 90 percentna sanca na poziar
                        //biotop[x][y + 1].setStav(POZIAR);
                        setFlame(x, y + 1);
                    }
                }
            }
        /************************ZAPAD**********************/
        } else if(this->vietor == ZAPAD) {
            if (positionIsValid(x - 1, y)) {
                if (biotop[x - 1][y].isFlamable()) {
                    int pravdepodobnostPoziaru = dis(gen);
                    if (pravdepodobnostPoziaru <= 20) { // 20 percentna sanca na poziar
                        //biotop[x - 1][y].setStav(POZIAR);
                        setFlame(x - 1, y);
                    }
                }
            }
            if (positionIsValid(x + 1, y)) {
                if (biotop[x + 1][y].isFlamable()) {
                    int pravdepodobnostPoziaru = dis(gen);
                    if (pravdepodobnostPoziaru <= 20) { // 2 percentna sanca na poziar
                        //biotop[x + 1][y].setStav(POZIAR);
                        setFlame(x + 1, y);
                    }
                }
            }
            if (positionIsValid(x, y - 1)) {
                if (biotop[x][y - 1].isFlamable()) {
                    int pravdepodobnostPoziaru = dis(gen);
                    if (pravdepodobnostPoziaru <= 90) { // 90 percentna sanca na poziar
                        //biotop[x][y - 1].setStav(POZIAR);
                        setFlame(x, y - 1);
                    }
                }
            }
            if (positionIsValid(x, y + 1)) {
                if (biotop[x][y + 1].isFlamable()) {
                    int pravdepodobnostPoziaru = dis(gen);
                    if (pravdepodobnostPoziaru <= 2) { // 2 percentna sanca na poziar
                        //biotop[x][y + 1].setStav(POZIAR);
                        setFlame(x, y + 1);
                    }
                }
            }
        }
    }

    Biotop* findBiotopNear(int x, int y,BiotopEnum biotopEnum) {
        if (!positionIsValid(x, y)) {
            std::cout << "Zadana pozicia nie je validna" << std::endl;
            return nullptr;
        }
        //pozri okolo
        if (positionIsValid(x - 1, y)) {
            if (biotop[x - 1][y].getStav() == biotopEnum) {
                return &biotop[x - 1][y];
            }
        }
        if (positionIsValid(x + 1, y)) {
            if (biotop[x + 1][y].getStav() == biotopEnum) {
                return &biotop[x + 1][y];
            }
        }
        if (positionIsValid(x, y - 1)) {
            if (biotop[x][y - 1].getStav() == biotopEnum) {
                return &biotop[x][y - 1];
            }
        }
        if (positionIsValid(x, y + 1)) {
            if (biotop[x][y + 1].getStav() == biotopEnum) {
                return &biotop[x][y + 1];
            }
        }
        return nullptr;
    }

    void regenerateBiotop(int x, int y) {
        if (!positionIsValid(x,y)) {
            std::cout << "Zadana pozicia nie je validna" << std::endl;
            return;
        }
        int pravdepodobnost = dis(gen);
        if (this->biotop[x][y].getStav() == ZHORENA) {
            Biotop* foundBiotop = findBiotopNear(x, y, VODA);
            if (foundBiotop != nullptr) {
                if (pravdepodobnost <= 10) {
                    this->biotop[x][y].setStav(LUKA);
                }
            }
        }
        if (this->biotop[x][y].getStav()==LUKA) {
            Biotop* foundBiotop = findBiotopNear(x, y, LES);
            if (foundBiotop != nullptr) {
                if (pravdepodobnost <= 2) {
                    this->biotop[x][y].setStav(LES);
                }
            }
        }
    }


    void step() {
        windStep(); // chod vetra
        //find fire
        for(int i = 0; i < sizeX; i++) {
            for(int j = 0; j < sizeY; j++) {
                if (biotop[i][j].getStav() == POZIAR) {
                    setFireArround(i, j);
                    //biotop[i][j].setZaciatokHorenia(this->pocetSimulacii);
                    if (this->biotop[i][j].getZaciatokHorenia() + 5 <= this->pocetSimulacii) {
                        biotop[i][j].setStav(ZHORENA);
                    }
                }
                regenerateBiotop(i, j);
            }
        }
        this->pocetSimulacii++;
    }

    void saveFile(const char* fileName) {
        std::ofstream file;
        file.open(fileName);
        if (!file.is_open()) {
            std::cerr << "invalid file name" << std::endl;
            return;
        }

        file << this->sizeX << " " << this->sizeY<< " "  << this->vietor << " ";

        for(int i = 0; i < this->sizeX; i++) {
            for(int j = 0; j < this->sizeY; j++) {
                file << biotop[i][j].getStav() << " ";
            }
        }

        file.close();
        std::cout << "Simulation saved!" << std::endl;
    }

    bool loadFile(const char* fileName) {
        std::ifstream file;
        file.open(fileName);
        if (!file.is_open()) {
            std::cerr << "invalid file name" << std::endl;
            return false;
        }
        // vymazeme najprv mapu aby sa vypraznila pamat
        for (int i = 0; i < sizeX; ++i) {
            delete[] biotop[i];
        }
        delete[] biotop;
        // to to treba keby nahodou ta nacitana mapa bola mensia ako ta starsia aby neboli memory leaky lebo by ostali v pamati stare data

        int nacitanyVietor;
        file >> this->sizeX >> this->sizeY >> nacitanyVietor;
        this->vietor = static_cast<VietorEnum>(nacitanyVietor);

        // vytvorime novu mapu z nacitanych hodnot
        biotop = new Biotop*[this->sizeX];
        // nastavime stavu bunky
        for(int i = 0; i < this->sizeX; i++) {
            biotop[i] = new Biotop[this->sizeY];
            for(int j = 0; j < this->sizeY; j++) {
                int nacitanyBiotop;
                file >> nacitanyBiotop;
                BiotopEnum spracovanyBiotop= static_cast<BiotopEnum>(nacitanyBiotop);
                biotop[i][j].setStav(spracovanyBiotop);
            }
        }
        std::cout << "Simulation loaded!" << std::endl;
        return true;
    }

    void runMutexLogic() {
        while(true) {
            {
                std::unique_lock<std::mutex> lock(mutex);
                cv.wait(lock, [this] {return zadanyZnak != "";});
                isPrinting = false;
                cv2.notify_one();
                lock.unlock();//
                if (zadanyZnak == "q") {
                    break;
                } else if (zadanyZnak == "f") {
                    int row, col;
                    std::cout << "Enter row and column for setFlame: ";
                    std::cin >> row >> col;
                    setFlame(row, col);
                } else if (zadanyZnak == "c") {
                    zadanyZnak = "";
                } else if (zadanyZnak == "s") {
                    std::cout << "Enter a filename to save the simulation : ";
                    std::string fileName;
                    std::cin >> fileName;
                    saveFile(fileName.c_str());
                }
                {
                    std::unique_lock<std::mutex> lock(mutex);
                    zadanyZnak = "";
                    isPrinting = true;
                }
                cv2.notify_one();
            }

        }

    }

    void printMutex() {
        //vykreslenie
        while(true) {
            {
                std::unique_lock<std::mutex> lock(mutex);
                cv2.wait(lock, [this] { return isPrinting == true; });
                consoleMutex.lock();
                this->step();
                this->print();
                consoleMutex.unlock();
                std::this_thread::sleep_for(std::chrono::seconds(3));
                cv2.notify_one();
            }
        }
    }

    void getUserInput() {
        while(true) {
            std::string userInput;
            if(std::cin >> userInput)
            {
                std::lock_guard<std::mutex> lock(mutex);
                zadanyZnak = userInput;
            }
            cv.notify_one();
        }
    }
};


#endif //SIMOHENKLIENT_SIMULACIA_H
