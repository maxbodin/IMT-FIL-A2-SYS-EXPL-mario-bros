#include <Applications/PlantsVsZombies/SnowPeaBullet.h>
#include <Applications/PlantsVsZombies/Zombie.h>
#include <Applications/PlantsVsZombies/sprites/objects/snowpeabullet_sprite.h>
#include <vga/vga.h>

SnowPeaBullet::SnowPeaBullet(int x, int y) : Bullet(x, y) {}

void SnowPeaBullet::update() {
    x += SPEED;
    if (x >= 320)
        deactivate();
}

void SnowPeaBullet::render() {
    draw_sprite(snowpeabullet_sprite_data,
                SNOWPEABULLET_WIDTH, SNOWPEABULLET_HEIGHT,
                x, y);
}

void SnowPeaBullet::erase() {
    plot_square(x, y, SNOWPEABULLET_WIDTH, 0);
}

int SnowPeaBullet::getWidth()  const { return SNOWPEABULLET_WIDTH; }
int SnowPeaBullet::getHeight() const { return SNOWPEABULLET_HEIGHT; }
int SnowPeaBullet::getDamage() const { return DAMAGE; }

void SnowPeaBullet::onHit(Zombie& target) {
    target.takeDamage(getDamage());
    target.applySlow(SLOW_DURATION);
}
