#ifndef PEASHOOTER_H
#define PEASHOOTER_H

#include <Applications/PlantsVsZombies/Entity.h>

class Bullet;

class Peashooter : public Entity {
public:
    static const int HP          = 300;
    static const int SHOOT_DELAY = 150; // ticks entre chaque tir
    static const int COST        = 100; // soleils nécessaires pour placer

    Peashooter(int x, int y);

    void update();
    void render();

    bool canShoot() const;
    void resetCooldown();
    virtual Bullet* createBullet(int bx, int by);

    int getWidth() const override;
    int getHeight() const override;

protected:
    static const int ANIM_SPEED = 10; // update calls entre chaque frame
    int cooldown;
    int frame;
    int animTick;
};

#endif
