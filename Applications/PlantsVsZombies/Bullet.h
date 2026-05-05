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

protected:
    int x, y;
    bool active;
};

#endif
