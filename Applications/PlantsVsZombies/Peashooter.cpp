#include <Applications/PlantsVsZombies/Peashooter.h>
#include <Applications/PlantsVsZombies/sprites/peashooter_sprite.h>
#include <vga/vga.h>

Peashooter::Peashooter(int x, int y)
    : Entity(x, y, HP), cooldown(SHOOT_DELAY) {}

void Peashooter::update() {
    if (state == DYING) {
        state = DEAD;
        return;
    }
    if (cooldown > 0)
        cooldown--;
}

void Peashooter::render() {
    if (state == DEAD) return;
    draw_sprite(peashooter_sprite_data, PEASHOOTER_WIDTH, PEASHOOTER_HEIGHT, x, y);
}

bool Peashooter::canShoot() const {
    return state == ALIVE && cooldown == 0;
}

void Peashooter::resetCooldown() {
    cooldown = SHOOT_DELAY;
}
