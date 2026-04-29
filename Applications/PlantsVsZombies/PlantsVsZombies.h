#ifndef PLANTSVSZOMBIES_H
#define PLANTSVSZOMBIES_H

#include <Applications/PlantsVsZombies/Grid.h>
#include <Applications/PlantsVsZombies/Peashooter.h>
#include <drivers/Clavier.h>

#define MAX_PLANTS 45

class PlantsVsZombies {
public:
    PlantsVsZombies();

    void init(Ecran* e,Clavier* c);
    void start();

private:
    Grid        grid;
    Peashooter* plants[MAX_PLANTS];
    int         plantCount;

    Ecran*      ecran;
    Clavier*    clavier;

    void update_screen();
};

#endif
