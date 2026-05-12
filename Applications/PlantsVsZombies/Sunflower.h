#ifndef SUNFLOWER_H
#define SUNFLOWER_H

#include <Applications/PlantsVsZombies/Peashooter.h>

class Sunflower : public Peashooter {
public:
    static const int COST        = 50;
    static const int SUN_MIN_DELAY = 3000;
    static const int SUN_MAX_DELAY = 6000;

    Sunflower(int x, int y);

    void update() override;
    void render() override;
    bool canShoot() const;
    Bullet* createBullet(int bx, int by) override;

    bool hasSunReady() const;
    void resetSunTimer();

    int getWidth()  const override;
    int getHeight() const override;

private:
    int nextSunTick;

    int randomSunDelay();
};

#endif
