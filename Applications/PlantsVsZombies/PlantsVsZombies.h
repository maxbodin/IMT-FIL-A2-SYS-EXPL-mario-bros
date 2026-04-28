#ifndef PLANTSVSZOMBIES_H
#define PLANTSVSZOMBIES_H

#include <Applications/PlantsVsZombies/Grid.h>
#include <Applications/PlantsVsZombies/Peashooter.h>

#define MAX_PLANTS 45

class PlantsVsZombies {
public:
    PlantsVsZombies();

    void init();
    void start();

private:
    Grid        grid;
    Peashooter* plants[MAX_PLANTS];
    int         plantCount;
};

#endif
