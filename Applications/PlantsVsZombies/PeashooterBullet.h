#ifndef PEASHOOTER_BULLET_H
#define PEASHOOTER_BULLET_H

#include <Applications/PlantsVsZombies/Bullet.h>

class PeashooterBullet : public Bullet {
public:
    PeashooterBullet(int x, int y);

    void update() override;
    void render() override;
    void erase() override;

    int getWidth()  const override;
    int getHeight() const override;
    int getDamage() const override;

private:
    static const int SPEED  = 4;
    static const int DAMAGE = 30;
};

#endif
