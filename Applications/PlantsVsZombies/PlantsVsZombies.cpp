#include <Applications/PlantsVsZombies/PlantsVsZombies.h>
#include <Applications/PlantsVsZombies/PeashooterBullet.h>
#include <Applications/PlantsVsZombies/sprites/shared_palette.h>
#include <Applications/PlantsVsZombies/sprites/peashooter_sprite.h>
#include <sextant/memoire/memoire.h>
#include <vga/vga.h>

extern volatile int compt;

PlantsVsZombies::PlantsVsZombies() : plantCount(0), bulletCount(0) {
    for (int i = 0; i < MAX_PLANTS; i++)
        plants[i] = 0;
    for (int i = 0; i < MAX_BULLETS; i++)
        bullets[i] = 0;
}

void PlantsVsZombies::init(Ecran* e,Clavier* c) {
    ecran = e;
    clavier = c;

    set_vga_mode13();
    set_palette_vga(shared_palette);
    clear_vga_screen(0);
}

void PlantsVsZombies::update_screen() {
    // update plants + spawn bullets + supprimer mortes
    for (int i = 0; i < plantCount; i++) {
        if (!plants[i]) continue;
        plants[i]->update();
        if (plants[i]->isDead()) {
            delete plants[i];
            plants[i] = plants[--plantCount];
            plants[plantCount] = 0;
            i--;
            continue;
        }
        if (plants[i]->canShoot() && bulletCount < MAX_BULLETS) {
            int bx = plants[i]->getX() + PEASHOOTER_WIDTH;
            int by = plants[i]->getY() + PEASHOOTER_HEIGHT / 2;
            bullets[bulletCount++] = new PeashooterBullet(bx, by);
            plants[i]->resetCooldown();
        }
    }

    // update bullets, supprimer inactives
    for (int i = 0; i < bulletCount; i++) {
        bullets[i]->erase();
        bullets[i]->update();
        if (!bullets[i]->isActive()) {
            delete bullets[i];
            bullets[i] = bullets[--bulletCount];
            bullets[bulletCount] = 0;
            i--;
        }
    }

    // rendu : fond puis entités
    grid.render();
    for (int i = 0; i < plantCount; i++)
        if (plants[i]) plants[i]->render();
    for (int i = 0; i < bulletCount; i++)
        if (bullets[i]) bullets[i]->render();
}

void PlantsVsZombies::start() {
    grid.render();

    int px, py;
    grid.tileToPixel(0, 0, px, py);
    plants[0] = new Peashooter(px, py);
    plantCount = 1;
    plants[0]->render();

    int lastTick  = compt;
    int renderFrames = 0;
    int fpsTimer  = compt;
    int fps       = 0;

    while (true) {
        while (compt == lastTick);
        lastTick = compt;

        if ((compt % 16) == 0) {
            // plants[0]->update();
            // grid.renderTile(0, 0);
            // plants[0]->render();

            update_screen();
            renderFrames++;
        }

        if (compt - fpsTimer >= 1000) {
            fps          = renderFrames;
            renderFrames = 0;
            fpsTimer     = compt;

            int seconds = compt / 1000;

            // secondes écoulées
            plot_square(0, 0, 14, 0);
            draw_number(seconds, 0, 2, 15, 2);

            // FPS
            plot_square(290, 0, 30, 0);
            draw_number(fps, 290, 2, 15, 2);
        }
    }
}
