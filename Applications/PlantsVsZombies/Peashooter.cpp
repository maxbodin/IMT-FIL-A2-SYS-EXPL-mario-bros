#include <Applications/PlantsVsZombies/Peashooter.h>
#include <Applications/PlantsVsZombies/PeashooterBullet.h>
#include <Applications/PlantsVsZombies/sprites/peashooter_sprite.h>
#include <vga/vga.h>

Peashooter::Peashooter(int x, int y)
    : Entity(x, y, HP), cooldown(SHOOT_DELAY), frame(0), animTick(0) {}

void Peashooter::update() {
    if (state == DYING) {
        state = DEAD;
        return;
    }
    if (cooldown > 0)
        cooldown--;

    if (++animTick >= ANIM_SPEED) {
        animTick = 0;
        frame = (frame + 1) % PEASHOOTER_FRAMES;
    }
}

void Peashooter::render() {
    if (state == DEAD) return;
    draw_sprite(peashooter_frames[frame], PEASHOOTER_WIDTH, PEASHOOTER_HEIGHT, x, y);
    renderHpBar(PEASHOOTER_WIDTH / 2, PEASHOOTER_HEIGHT);
}

bool Peashooter::canShoot() const {
    return state == ALIVE && cooldown == 0;
    // return false; // pour tester les zombies
}

void Peashooter::resetCooldown() {
    cooldown = SHOOT_DELAY;
}

int Peashooter::getWidth() const {
    return PEASHOOTER_WIDTH;
}

int Peashooter::getHeight() const {
    return PEASHOOTER_HEIGHT;
}

Bullet* Peashooter::createBullet(int bx, int by) {
    return new PeashooterBullet(bx, by);
}

bool Peashooter::hasSunReady() const { return false; }
void Peashooter::resetSunTimer() {}
