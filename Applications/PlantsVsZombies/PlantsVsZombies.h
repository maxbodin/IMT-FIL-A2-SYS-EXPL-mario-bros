#ifndef PLANTSVSZOMBIES_H
#define PLANTSVSZOMBIES_H

#include <Applications/PlantsVsZombies/Grid.h>
#include <Applications/PlantsVsZombies/Peashooter.h>

#define MAX_PLANTS  45
#define MAX_ZOMBIES 20

class PlantsVsZombies {
public:
    PlantsVsZombies();

    void init();
    void update();
    void render();

    void placePlant(int col, int row);

private:
    Grid       _grid;
    Peashooter* _plants[MAX_PLANTS];
    int        _plantCount;

    void updateEntities();
    void renderEntities();
    void cleanDeadEntities();
};

#endif
