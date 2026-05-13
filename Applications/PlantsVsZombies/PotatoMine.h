#ifndef POTATO_MINE_H
#define POTATO_MINE_H

#include <Applications/PlantsVsZombies/Peashooter.h>

class PotatoMine : public Peashooter {
public:
    static const int COST               = 25;
    static const int EXPLOSION_DAMAGE   = 200;
    static const int EXPLOSION_DURATION = 400;  // ticks for explosion animation

    PotatoMine(int x, int y);

    void update() override;
    void render() override;
    bool canShoot() const override;
    BulletType getBulletType() const override;
    PlantType getPlantType() const override;
    bool hasSunReady() const override;
    void resetSunTimer() override;

    int getWidth()  const override;
    int getHeight() const override;

    /* Trigger explosion (called when zombie steps on mine). */
    void explode();
    bool isExploding()    const { return exploding; }
    bool isExplosionDone() const { return explosionDone; }
    int  getExplosionDamage() const { return EXPLOSION_DAMAGE; }

private:
    bool exploding;
    bool explosionDone;
    int  explosionStartTick;
    int  explosionFrame;
    int  explosionAnimTick;
};

#endif
