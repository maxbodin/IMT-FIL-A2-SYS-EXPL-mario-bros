#ifndef SNOW_PEASHOOTER_H
#define SNOW_PEASHOOTER_H

#include <Applications/PlantsVsZombies/Peashooter.h>

class SnowPeashooter : public Peashooter {
public:
    static const int COST = 175;

    SnowPeashooter(int x, int y);

    void update() override;
    void render() override;
    Bullet* createBullet(int bx, int by) override;
};

#endif
