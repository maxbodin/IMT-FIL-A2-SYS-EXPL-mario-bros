#ifndef WALLNUT_H
#define WALLNUT_H

#include <Applications/PlantsVsZombies/Peashooter.h>

class WallNut : public Peashooter {
public:
    static const int COST = 50;
    static const int WALLNUT_HP = 800;

    WallNut(int x, int y);

    void update() override;
    void render() override;
    bool canShoot() const override;
    BulletType getBulletType() const override;
    PlantType getPlantType() const override;
    bool hasSunReady() const override;
    void resetSunTimer() override;

    int getWidth()  const override;
    int getHeight() const override;
};

#endif
