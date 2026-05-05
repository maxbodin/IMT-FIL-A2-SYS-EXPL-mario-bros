#include <Applications/PlantsVsZombies/Entity.h>

int Entity::nextId = 0;

Entity::Entity(int x, int y, int hp)
    : _id(nextId++), x(x), y(y), hp(hp), state(ALIVE) {}

void Entity::takeDamage(int amount) {
    if (state != ALIVE) return;
    hp -= amount;
    if (hp <= 0) {
        hp = 0;
        state = DYING;
    }
}

bool Entity::isDead() const {
    return state == DEAD;
}

int Entity::getId() const {
    return _id;
}

EntityState Entity::getState() const {
    return state;
}

int Entity::getX() const { return x; }
int Entity::getY() const { return y; }
