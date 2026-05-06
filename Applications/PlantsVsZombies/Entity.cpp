#include <Applications/PlantsVsZombies/Entity.h>
#include <vga/vga.h>

int Entity::nextId = 0;

Entity::Entity(int x, int y, int hp)
    : _id(nextId++), x(x), y(y), hp(hp), maxHp(hp), state(ALIVE) {}

void Entity::takeDamage(int amount) {
    if (state == DYING || state == DEAD) return;
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

int Entity::getWidth() const { return 0; } // default, override in derived classes
int Entity::getHeight() const { return 0; } // default, override in derived classes

int Entity::getHp() const { return hp; }
int Entity::getMaxHp() const { return maxHp; }

// palette index 205 = (41,63,44) green-ish, 50 = (23,1,0) dark red
void Entity::renderHpBar(int barW, int spriteH) const {
    if (state == DEAD) return;
    int barX = x + barW / 2;
    int barY = y - 5;
    draw_hp_bar(barX, barY, barW, hp, maxHp);
}
