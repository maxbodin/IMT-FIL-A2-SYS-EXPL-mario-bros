#ifndef BULLET_H
#define BULLET_H

#include <Applications/PlantsVsZombies/Entity.h>

class Bullet : public Entity {
public:
    Bullet(int x, int y);

    void update() override;
    void render() override;

private:
    static const int HP = 1;
    static const int SPEED = 4;
};

#endif
