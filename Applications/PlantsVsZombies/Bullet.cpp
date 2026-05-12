#include <Applications/PlantsVsZombies/Bullet.h>
#include <Applications/PlantsVsZombies/Zombie.h>
#include <Applications/PlantsVsZombies/sprites/peashooter_bullet_sprite.h>
#include <vga/vga.h>

/* Damage values per bullet type. */
static const int DAMAGE_PEASHOOTER = 30;
static const int DAMAGE_SNOW_PEA   = 25;

Bullet::Bullet() : x(0), y(0), spawnX(0), active(false),
                   type(BULLET_PEASHOOTER), damage(0) {}

void Bullet::init(int x, int y, BulletType type) {
    this->x      = x;
    this->y      = y;
    this->spawnX = x;
    this->active = true;
    this->type   = type;
    this->damage = (type == BULLET_SNOW_PEA) ? DAMAGE_SNOW_PEA : DAMAGE_PEASHOOTER;
}

void Bullet::update() {
    if (!active) return;
    x += SPEED;
    if (x >= 320)
        active = false;
}

void Bullet::render() {
    if (!active) return;
    draw_sprite(peashooter_bullet_sprite_data,
                PEASHOOTER_BULLET_WIDTH, PEASHOOTER_BULLET_HEIGHT,
                x, y);
}

void Bullet::onHit(Zombie& target) {
    target.takeDamage(damage);
    if (type == BULLET_SNOW_PEA)
        target.applySlow(SLOW_DURATION);
}

bool Bullet::isActive()   const { return active; }
void Bullet::deactivate()       { active = false; }
int  Bullet::getX()       const { return x; }
int  Bullet::getY()       const { return y; }
int  Bullet::getSpawnX()  const { return spawnX; }
int  Bullet::getWidth()   const { return PEASHOOTER_BULLET_WIDTH; }
int  Bullet::getHeight()  const { return PEASHOOTER_BULLET_HEIGHT; }
int  Bullet::getDamage()  const { return damage; }
