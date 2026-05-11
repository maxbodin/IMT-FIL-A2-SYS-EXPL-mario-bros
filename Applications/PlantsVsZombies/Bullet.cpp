#include <Applications/PlantsVsZombies/Bullet.h>
#include <Applications/PlantsVsZombies/Zombie.h>

Bullet::Bullet(int x, int y) : x(x), y(y), spawnX(x), active(true) {}

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
int Bullet::getSpawnX() const { return spawnX; }

void Bullet::onHit(Zombie& target) {
    target.takeDamage(getDamage());
}
