#include <Applications/PlantsVsZombies/Entity.h>
#include <vga/vga.h>

int Entity::nextId = 0;

Entity::Entity(int x, int y, int hp)
    : _id(nextId++), x(x), y(y), hp(hp), maxHp(hp), state(ALIVE) {}

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
int Entity::getHp() const { return hp; }
int Entity::getMaxHp() const { return maxHp; }

// palette index 205 = (41,63,44) green-ish, 50 = (23,1,0) dark red
void Entity::renderHpBar(int barW, int spriteH) const {
    if (state == DEAD) return;
    int barY = y + spriteH + 2;
    draw_hp_bar(x, barY, barW, hp, maxHp);
}
