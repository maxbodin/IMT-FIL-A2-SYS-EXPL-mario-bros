#include <Applications/PlantsVsZombies/Bullet.h>

Bullet::Bullet(int x, int y) : x(x), y(y), active(true) {}

bool Bullet::isActive() { 
    return active; 
}
void Bullet::deactivate() { 
    active = false; 
}

int Bullet::getX() { 
    return x; 
}
int Bullet::getY() { 
    return y; 
}
