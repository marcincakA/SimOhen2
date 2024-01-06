//
// Created by marci on 1/3/2024.
//

#ifndef SIMOHENKLIENT_BIOTOP_H
#define SIMOHENKLIENT_BIOTOP_H

enum BiotopEnum {
    LES = 0,
    LUKA = 1,
    SKALA = 2,
    VODA = 3,
    POZIAR = 4,
    ZHORENA = 5
};


class Biotop {
private:
    int posX, posY;
    BiotopEnum stav;
    int zaciatokHorenia;
    bool zhorena;
public:
    Biotop() {
        this->posX = 0;
        this->posY = 0;
        this->stav = LES;
        this->zhorena = false;
        this->zaciatokHorenia = 0;
    }
    Biotop(int posX,int posY,BiotopEnum stav) {
        this->posX = posX;
        this->posY = posY;
        this->stav = stav;
        this->zhorena = false;
        this->zaciatokHorenia = 0;
    }
    int getPosX() const {
        return posX;
    }
    int getPosY() const {
        return posY;
    }
    BiotopEnum getStav() const {
        return stav;
    }
    bool isZhorena() const {
        return zhorena;
    }
    void setZhorena(bool zhorena) {
        this->zhorena = zhorena;
    }
    void setStav(BiotopEnum stav) {
        this->stav = stav;
    }
    void setPosX(int posX) {
        this->posX = posX;
    }
    void setPosY(int posY) {
        this->posY = posY;
    }

    bool isFlamable() {
        return stav == LES || stav == LUKA;
    }
    //getter setter
    int getZaciatokHorenia() {
        return this->zaciatokHorenia;
    }
    void setZaciatokHorenia(int Horenia) {
        this->zaciatokHorenia = Horenia;
    }
};


#endif //SIMOHENKLIENT_BIOTOP_H
