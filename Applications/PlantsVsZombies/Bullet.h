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
    void deactivate();

    int getX() const;
    int getY() const;
    int getSpawnX() const;
    int getWidth()  const;
    int getHeight() const;
    int getDamage() const;

private:
    static const int SPEED         = 4;
    static const int SLOW_DURATION = 300;

    int x, y;
    int spawnX;
    bool active;
    BulletType type;
    int damage;
};

#endif
