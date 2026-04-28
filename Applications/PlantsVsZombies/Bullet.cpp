#include <Applications/PlantsVsZombies/Bullet.h>
#include <Applications/PlantsVsZombies/sprites/peashooter_bullet_sprite.h>
#include <vga/vga.h>

Bullet::Bullet(int x, int y) : Entity(x, y, HP) {}

void Bullet::update() {
    x += SPEED;
    if (x >= 320)
        state = DEAD;
}

void Bullet::render() {
    draw_sprite(peashooter_bullet_sprite_data,
                PEASHOOTER_BULLET_WIDTH, PEASHOOTER_BULLET_HEIGHT,
                x, y);
}
