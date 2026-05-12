#ifndef SNOW_PEA_BULLET_H
#define SNOW_PEA_BULLET_H

#include <Applications/PlantsVsZombies/Bullet.h>

class SnowPeaBullet : public Bullet {
public:
    SnowPeaBullet(int x, int y);

    void update() override;
    void render() override;
    void erase() override;

    int getWidth()  const override;
    int getHeight() const override;
    int getDamage() const override;
    void onHit(Zombie& target) override;

private:
    static const int SPEED         = 4;
    static const int DAMAGE        = 25;
    static const int SLOW_DURATION = 300; // ticks de ralentissement
};

#endif
