#include <Applications/PlantsVsZombies/PeashooterBullet.h>
#include <Applications/PlantsVsZombies/sprites/objects/peabullet_sprite.h>
#include <vga/vga.h>

PeashooterBullet::PeashooterBullet(int x, int y) : Bullet(x, y) {}

void PeashooterBullet::update() {
    x += SPEED;
    if (x + PEASHOOTER_BULLET_WIDTH >= 320)
        deactivate();
}

void PeashooterBullet::render() {
    draw_sprite(peabullet_sprite_data,
                PEABULLET_WIDTH, PEABULLET_HEIGHT,
                x, y);
}

void PeashooterBullet::erase() {
    plot_square(x, y, PEABULLET_WIDTH, 0);
}

int PeashooterBullet::getWidth()  const { return PEABULLET_WIDTH; }
int PeashooterBullet::getHeight() const { return PEABULLET_HEIGHT; }
int PeashooterBullet::getDamage() const { return DAMAGE; }
