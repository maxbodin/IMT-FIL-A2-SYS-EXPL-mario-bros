#include <Applications/PlantsVsZombies/PlantsVsZombies.h>
#include <Applications/PlantsVsZombies/sprites/shared_palette.h>
#include <sextant/memoire/memoire.h>
#include <vga/vga.h>

extern volatile int compt;

PlantsVsZombies::PlantsVsZombies() : plantCount(0), bulletCount(0) {
    for (int i = 0; i < MAX_PLANTS; i++)
        plants[i] = 0;
    for (int i = 0; i < MAX_BULLETS; i++)
        bullets[i] = 0;
}

void PlantsVsZombies::init() {
    set_vga_mode13();
    set_palette_vga(shared_palette);
    clear_vga_screen(0);
}

void PlantsVsZombies::start() {
    grid.render();

    int px, py;
    grid.tileToPixel(0, 0, px, py);
    plants[0] = new Peashooter(px, py);
    plantCount = 1;

    int lastTick = compt;
    while (true) {
        while (compt - lastTick < 16);
        lastTick = compt;
        update();
        grid.render();
        render();
    }
}

void PlantsVsZombies::update() {
    for (int i = 0; i < plantCount; i++) {
        if (!plants[i]) continue;
        plants[i]->update();

        if (plants[i]->canShoot() && bulletCount < MAX_BULLETS) {
            int bx = plants[i]->getX() + 28;
            int by = plants[i]->getY() + 12;
            bullets[bulletCount++] = new Bullet(bx, by);
            plants[i]->resetCooldown();
        }
    }

    for (int i = 0; i < bulletCount; i++) {
        if (!bullets[i]) continue;
        bullets[i]->update();
        if (bullets[i]->isDead()) {
            delete bullets[i];
            bullets[i] = bullets[--bulletCount];
            bullets[bulletCount] = 0;
        }
    }
}

void PlantsVsZombies::render() {
    for (int i = 0; i < plantCount; i++)
        if (plants[i]) plants[i]->render();
    for (int i = 0; i < bulletCount; i++)
        if (bullets[i]) bullets[i]->render();
}
