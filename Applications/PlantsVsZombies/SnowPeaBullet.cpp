#include <Applications/PlantsVsZombies/SnowPeaBullet.h>
#include <Applications/PlantsVsZombies/Zombie.h>
#include <Applications/PlantsVsZombies/sprites/peashooter_bullet_sprite.h>
#include <vga/vga.h>

SnowPeaBullet::SnowPeaBullet(int x, int y) : Bullet(x, y) {}

void SnowPeaBullet::update() {
    x += SPEED;
    if (x >= 320)
        deactivate();
}

void SnowPeaBullet::render() {
    // Reuse peashooter bullet sprite for the moment
    draw_sprite(peashooter_bullet_sprite_data,
                PEASHOOTER_BULLET_WIDTH, PEASHOOTER_BULLET_HEIGHT,
                x, y);
}

void SnowPeaBullet::erase() {
    plot_square(x, y, PEASHOOTER_BULLET_WIDTH, 0);
}

int SnowPeaBullet::getWidth()  const { return PEASHOOTER_BULLET_WIDTH; }
int SnowPeaBullet::getHeight() const { return PEASHOOTER_BULLET_HEIGHT; }
int SnowPeaBullet::getDamage() const { return DAMAGE; }

void SnowPeaBullet::onHit(Zombie& target) {
    target.takeDamage(getDamage());
    target.applySlow(SLOW_DURATION);
}
