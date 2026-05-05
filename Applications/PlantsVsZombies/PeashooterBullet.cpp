#include <Applications/PlantsVsZombies/PeashooterBullet.h>
#include <Applications/PlantsVsZombies/sprites/peashooter_bullet_sprite.h>
#include <vga/vga.h>

PeashooterBullet::PeashooterBullet(int x, int y) : Bullet(x, y) {}

void PeashooterBullet::update() {
    x += SPEED;
    if (x >= 320)
        deactivate();
}

void PeashooterBullet::render() {
    draw_sprite(peashooter_bullet_sprite_data,
                PEASHOOTER_BULLET_WIDTH, PEASHOOTER_BULLET_HEIGHT,
                x, y);
}

void PeashooterBullet::erase() {
    plot_square(x, y, PEASHOOTER_BULLET_WIDTH, 0);
}
