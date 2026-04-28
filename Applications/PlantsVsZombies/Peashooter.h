#ifndef PEASHOOTER_H
#define PEASHOOTER_H

#include <Applications/PlantsVsZombies/Entity.h>

class Peashooter : public Entity {
public:
    static const int HP          = 300;
    static const int SHOOT_DELAY = 150; // ticks entre chaque tir

    Peashooter(int x, int y);

    void update();
    void render();

    bool canShoot() const;
    void resetCooldown();

private:
    int cooldown;
};

#endif
