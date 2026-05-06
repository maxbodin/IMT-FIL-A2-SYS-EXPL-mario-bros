#ifndef BULLET_H
#define BULLET_H

class Bullet {
public:
    Bullet(int x, int y);

    virtual void update() = 0;
    virtual void render() = 0;
    virtual void erase() = 0;

    bool isActive();
    void deactivate();

    int getX();
    int getY();
    int getSpawnX() const;
    virtual int getWidth()  const = 0;
    virtual int getHeight() const = 0;
    virtual int getDamage() const = 0;

protected:
    int x, y;
    int spawnX;
    bool active;
};

#endif
