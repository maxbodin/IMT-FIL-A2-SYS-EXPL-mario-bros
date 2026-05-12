#include <Applications/PlantsVsZombies/SnowPeashooter.h>
#include <Applications/PlantsVsZombies/sprites/snow_peashooter_sprite.h>
#include <vga/vga.h>

SnowPeashooter::SnowPeashooter(int x, int y) : Peashooter(x, y) {}

void SnowPeashooter::update() {
    if (state == DYING) { state = DEAD; return; }
    if (cooldown > 0) cooldown--;
    if (++animTick >= ANIM_SPEED) {
        animTick = 0;
        frame = (frame + 1) % SNOW_PEASHOOTER_FRAMES;
    }
}

void SnowPeashooter::render() {
    if (getState() == DEAD) return;
    draw_sprite(snow_peashooter_frames[frame], SNOW_PEASHOOTER_WIDTH, SNOW_PEASHOOTER_HEIGHT, x, y);
    renderHpBar(SNOW_PEASHOOTER_WIDTH / 2, SNOW_PEASHOOTER_HEIGHT);
}

BulletType SnowPeashooter::getBulletType() const {
    return BULLET_SNOW_PEA;
}
