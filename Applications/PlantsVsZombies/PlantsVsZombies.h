#ifndef PLANTSVSZOMBIES_H
#define PLANTSVSZOMBIES_H

#include <Applications/PlantsVsZombies/Grid.h>
#include <Applications/PlantsVsZombies/Peashooter.h>
#include <Applications/PlantsVsZombies/Bullet.h>

#define MAX_PLANTS  45
#define MAX_BULLETS 90

class PlantsVsZombies {
public:
    PlantsVsZombies();

    void init();
    void start();

private:
    void update();
    void render();

    Grid        grid;
    Peashooter* plants[MAX_PLANTS];
    int         plantCount;
    Bullet*     bullets[MAX_BULLETS];
    int         bulletCount;
};

#endif
