#include <Applications/PlantsVsZombies/Bullet.h>
#include <Applications/PlantsVsZombies/Zombie.h>
#include <Applications/PlantsVsZombies/sprites/objects/peabullet_sprite.h>
#include <Applications/PlantsVsZombies/sprites/objects/snowpeabullet_sprite.h>
#include <Applications/PlantsVsZombies/sprites/objects/peabullet_impact_sprite.h>
#include <Applications/PlantsVsZombies/sprites/objects/snowpea_impact_sprite.h>
#include <vga/vga.h>

/* Damage values per bullet type. */
static const int DAMAGE_PEASHOOTER = 30;
static const int DAMAGE_SNOW_PEA   = 25;

Bullet::Bullet() : x(0), y(0), spawnX(0), active(false),
                   type(BULLET_PEASHOOTER), damage(0),
                   impacting(false), impactFrame(0), impactAnimTick(0), impactHoldTicks(0) {}

void Bullet::init(int x, int y, BulletType type) {
    this->x      = x;
    this->y      = y;
    this->spawnX = x;
    this->active = true;
    this->type   = type;
    this->damage = (type == BULLET_SNOW_PEA) ? DAMAGE_SNOW_PEA : DAMAGE_PEASHOOTER;
    this->impacting = false;
    this->impactFrame = 0;
    this->impactAnimTick = 0;
    this->impactHoldTicks = 0;
}

void Bullet::update() {
    if (!active) return;

    if (impacting) {
        impactHoldTicks--;
        if (++impactAnimTick >= IMPACT_ANIM_SPEED) {
            impactAnimTick = 0;
            impactFrame++;
        }
        if (impactHoldTicks <= 0) {
            active = false;
            impacting = false;
        }
        return;
    }

    x += SPEED;
    if (x >= 320)
        active = false;
}

void Bullet::render() {
    if (!active) return;

    if (impacting) {
        if (type == BULLET_SNOW_PEA) {
            int f = impactFrame < SNOWPEA_IMPACT_FRAMES ? impactFrame : SNOWPEA_IMPACT_FRAMES - 1;
            draw_sprite(snowpea_impact_frames[f],
                        SNOWPEA_IMPACT_WIDTH, SNOWPEA_IMPACT_HEIGHT, x, y);
        } else {
            int f = impactFrame < PEABULLET_IMPACT_FRAMES ? impactFrame : PEABULLET_IMPACT_FRAMES - 1;
            draw_sprite(peabullet_impact_frames[f],
                        PEABULLET_IMPACT_WIDTH, PEABULLET_IMPACT_HEIGHT, x, y);
        }
        return;
    }

    if (type == BULLET_SNOW_PEA) {
        draw_sprite(snowpeabullet_sprite_data,
                    SNOWPEABULLET_WIDTH, SNOWPEABULLET_HEIGHT,
                    x, y);
    } else {
        draw_sprite(peabullet_sprite_data,
                    PEABULLET_WIDTH, PEABULLET_HEIGHT,
                    x, y);
    }
}

void Bullet::onHit(Zombie& target) {
    target.takeDamage(damage);
    if (type == BULLET_SNOW_PEA)
        target.applySlow(SLOW_DURATION);
}

void Bullet::startImpact() {
    impacting = true;
    impactFrame = 0;
    impactAnimTick = 0;
    impactHoldTicks = IMPACT_HOLD;
}

bool Bullet::isActive()    const { return active; }
bool Bullet::isImpacting() const { return impacting; }
void Bullet::deactivate()        { active = false; impacting = false; }
int  Bullet::getX()       const { return x; }
int  Bullet::getY()       const { return y; }
int  Bullet::getSpawnX()  const { return spawnX; }
int  Bullet::getWidth()   const { return PEABULLET_WIDTH; }
int  Bullet::getHeight()  const { return PEABULLET_HEIGHT; }
int  Bullet::getDamage()  const { return damage; }
BulletType Bullet::getType() const { return type; }
