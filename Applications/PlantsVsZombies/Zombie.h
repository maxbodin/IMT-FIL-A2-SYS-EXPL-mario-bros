#ifndef ZOMBIE_H
#define ZOMBIE_H

#include <Applications/PlantsVsZombies/Entity.h>

class Zombie : public Entity {
public:
    static const int HP        = 300;
    static const int HIT_DELAY = 150; // ticks entre chaque coup

    Zombie(int x, int y);

    void update();
    void render();

    bool canHit() const;
    void resetCooldown();

    int getWidth()  const override;
    int getHeight() const override;

    void block();
    void unblock();
    bool isBlocked() const;

    void applySlow(int duration);
    bool isSlowed() const;

private:
    static const int ANIM_SPEED = 15; // update calls entre chaque frame
    int cooldown;
    int frame;
    int animTick;
    int slowTicks;
};

#endif