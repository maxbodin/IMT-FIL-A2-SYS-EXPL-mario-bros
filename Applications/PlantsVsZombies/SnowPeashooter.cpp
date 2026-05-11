#include <Applications/PlantsVsZombies/SnowPeashooter.h>
#include <Applications/PlantsVsZombies/SnowPeaBullet.h>
#include <Applications/PlantsVsZombies/sprites/snow_peashooter_sprite.h>
#include <vga/vga.h>

SnowPeashooter::SnowPeashooter(int x, int y) : Peashooter(x, y) {}

void SnowPeashooter::render() {
    if (getState() == DEAD) return;
    draw_sprite(snow_peashooter_sprite_data, SNOW_PEASHOOTER_WIDTH, SNOW_PEASHOOTER_HEIGHT, x, y);
    renderHpBar(SNOW_PEASHOOTER_WIDTH / 2, SNOW_PEASHOOTER_HEIGHT);
}

Bullet* SnowPeashooter::createBullet(int bx, int by) {
    return new SnowPeaBullet(bx, by);
}
