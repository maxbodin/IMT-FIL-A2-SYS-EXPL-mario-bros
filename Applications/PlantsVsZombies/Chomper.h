#ifndef CHOMPER_H
#define CHOMPER_H

#include <Applications/PlantsVsZombies/Peashooter.h>

enum ChomperState { CHOMP_IDLE, CHOMP_ATTACKING, CHOMP_CHEWING };

class Chomper : public Peashooter {
public:
    static const int COST       = 150;
    static const int CHEW_TIME  = 15000; // ~15s to digest

    Chomper(int x, int y);

    void update();
    void render();

    bool canShoot() const;
    BulletType getBulletType() const;
    PlantType getPlantType() const;

    ChomperState getChomperState() const;
    void startAttack();
    bool isChewing() const;

    int getWidth() const override;
    int getHeight() const override;

private:
    static const int ATTACK_ANIM_SPEED = 6;
    static const int CHEW_ANIM_SPEED   = 15;

    ChomperState chomperState;
    int attackFrame;
    int attackAnimTick;
    int chewFrame;
    int chewAnimTick;
    int chewEndTick;
};

#endif
