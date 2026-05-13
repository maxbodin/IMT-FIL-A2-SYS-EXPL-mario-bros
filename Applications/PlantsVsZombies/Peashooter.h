#ifndef PEASHOOTER_H
#define PEASHOOTER_H

#include <Applications/PlantsVsZombies/Entity.h>
#include <Applications/PlantsVsZombies/Bullet.h>
#include <Applications/PlantsVsZombies/PlantType.h>

class Peashooter : public Entity {
public:
    static const int HP          = 300;
    static const int SHOOT_DELAY = 150; // ticks entre chaque tir
    static const int COST        = 100; // soleils nécessaires pour placer

    Peashooter(int x, int y);

    void update();
    void render();

    virtual bool canShoot() const;
    void resetCooldown();
    virtual BulletType getBulletType() const;
    virtual bool hasSunReady() const;
    virtual void resetSunTimer();

    virtual PlantType getPlantType() const;

    int getWidth() const override;
    int getHeight() const override;

protected:
    static const int ANIM_SPEED = 10; // update calls entre chaque frame
    int cooldown;
    int frame;
    int animTick;
};

#endif
