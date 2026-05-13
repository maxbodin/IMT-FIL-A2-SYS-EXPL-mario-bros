#include <Applications/PlantsVsZombies/Peashooter.h>
#include <Applications/PlantsVsZombies/sprites/plants/peashooter_sprite.h>
#include <Applications/PlantsVsZombies/sprites/plants/peashooter_shooting_sprite.h>
#include <vga/vga.h>

Peashooter::Peashooter(int x, int y)
    : Entity(x, y, HP), cooldown(SHOOT_DELAY), frame(0), animTick(0),
      shooting(false), shootFrame(0), shootAnimTick(0), shootHoldTicks(0) {}

void Peashooter::update() {
    if (state == DYING) {
        state = DEAD;
        return;
    }
    if (cooldown > 0)
        cooldown--;

    if (shooting) {
        if (++shootAnimTick >= 6) {
            shootAnimTick = 0;
            shootFrame = (shootFrame + 1) % PEASHOOTER_SHOOTING_FRAMES;
        }
        shootHoldTicks--;
        if (shootHoldTicks <= 0) {
            shooting = false;
            frame = 0;
            animTick = 0;
        }
    } else {
        if (++animTick >= ANIM_SPEED) {
            animTick = 0;
            frame = (frame + 1) % PEASHOOTER_FRAMES;
        }
    }
}

void Peashooter::render() {
    if (state == DEAD) return;
    if (shooting) {
        draw_sprite(peashooter_shooting_frames[shootFrame],
                    PEASHOOTER_SHOOTING_WIDTH, PEASHOOTER_SHOOTING_HEIGHT, x, y);
        renderHpBar(PEASHOOTER_SHOOTING_WIDTH / 2, PEASHOOTER_SHOOTING_HEIGHT);
    } else {
        draw_sprite(peashooter_frames[frame], PEASHOOTER_WIDTH, PEASHOOTER_HEIGHT, x, y);
        renderHpBar(PEASHOOTER_WIDTH / 2, PEASHOOTER_HEIGHT);
    }
}

bool Peashooter::canShoot() const {
    return state == ALIVE && cooldown == 0;
}

void Peashooter::resetCooldown() {
    cooldown = SHOOT_DELAY;
    shooting = true;
    shootFrame = 0;
    shootAnimTick = 0;
    shootHoldTicks = SHOOT_ANIM_HOLD;
}

int Peashooter::getWidth() const {
    return PEASHOOTER_WIDTH;
}

int Peashooter::getHeight() const {
    return PEASHOOTER_HEIGHT;
}

BulletType Peashooter::getBulletType() const {
    return BULLET_PEASHOOTER;
}

PlantType Peashooter::getPlantType() const { return PLANT_PEASHOOTER; }

bool Peashooter::hasSunReady() const { return false; }
void Peashooter::resetSunTimer() {}
