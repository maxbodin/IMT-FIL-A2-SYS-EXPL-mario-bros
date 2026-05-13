#ifndef GATLING_PEA_H
#define GATLING_PEA_H

#include <Applications/PlantsVsZombies/Peashooter.h>

class GatlingPea : public Peashooter {
public:
    static const int COST        = 250;
    static const int SHOOT_DELAY = 30;  // much faster than peashooter

    GatlingPea(int x, int y);

    void update();
    void render();

    bool canShoot() const;
    void resetCooldown();
    BulletType getBulletType() const;
    PlantType getPlantType() const;

    int getWidth() const override;
    int getHeight() const override;

private:
    bool shooting;
    int shootFrame;
    int shootAnimTick;
    static const int SHOOT_ANIM_SPEED = 4;
    static const int SHOOT_ANIM_HOLD  = 24; // frames to stay in shooting anim
    int shootHoldTicks;
};

#endif
