#ifndef BULLET_H
#define BULLET_H

class Zombie;

enum BulletType { BULLET_PEASHOOTER, BULLET_SNOW_PEA };

class Bullet {
public:
    Bullet();

    void init(int x, int y, BulletType type);
    void update();
    void render();
    void onHit(Zombie& target);

    bool isActive() const;
    bool isImpacting() const;
    void deactivate();
    void startImpact();

    int getX() const;
    int getY() const;
    int getSpawnX() const;
    int getWidth()  const;
    int getHeight() const;
    int getDamage() const;
    BulletType getType() const;

private:
    static const int SPEED             = 4;
    static const int SLOW_DURATION     = 300;
    static const int IMPACT_ANIM_SPEED = 8;
    static const int IMPACT_HOLD       = 16;

    int x, y;
    int spawnX;
    bool active;
    BulletType type;
    int damage;
    bool impacting;
    int impactFrame;
    int impactAnimTick;
    int impactHoldTicks;
};

#endif
