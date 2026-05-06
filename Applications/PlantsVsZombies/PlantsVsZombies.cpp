#include <Applications/PlantsVsZombies/PlantsVsZombies.h>
#include <Applications/PlantsVsZombies/PeashooterBullet.h>
#include <Applications/PlantsVsZombies/sprites/shared_palette.h>
#include <Applications/PlantsVsZombies/sprites/peashooter_sprite.h>
#include <Applications/PlantsVsZombies/sprites/zombie_walk_sprite.h>
#include <sextant/memoire/memoire.h>
#include <vga/vga.h>

extern volatile int compt;

PlantsVsZombies::PlantsVsZombies() : plantCount(0), bulletCount(0), zombieCount(0) {
    for (int i = 0; i < MAX_PLANTS; i++)
        plants[i] = 0;
    for (int i = 0; i < MAX_BULLETS; i++)
        bullets[i] = 0;
    for (int i = 0; i < MAX_ZOMBIES; i++)
        zombies[i] = 0;
}

void PlantsVsZombies::init(Ecran* e,Clavier* c) {
    ecran = e;
    clavier = c;

    backbuffer = (unsigned char*) getmem(320 * 200);

    set_vga_mode13();
    set_palette_vga(shared_palette);
    clear_vga_screen(0);
}

void PlantsVsZombies::update_screen() {
    
    // update plants + spawn bullets
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
            int bx = plants[i]->getX() + plants[i]->getWidth();
            int by = plants[i]->getY() + plants[i]->getHeight() / 2;
            bullets[bulletCount++] = new PeashooterBullet(bx, by);
            plants[i]->resetCooldown();
        }
    }

    // update zombies
    for (int i = 0; i < zombieCount; i++) {
        if (!zombies[i]) continue;
        bool inFront = false;
        for (int p = 0; p < plantCount; p++) {
            if (!plants[p]) continue;
            int dx = zombies[i]->getX() - (plants[p]->getX() + plants[p]->getWidth());
            if (dx >= 0 && dx < COLLISION_DISTANCE)  {
                inFront = true;
                break;
            }
        }
        inFront ? zombies[i]->block() : zombies[i]->unblock();
        zombies[i]->update();
    }

    // update bullets, supprimer inactives
    for (int i = 0; i < bulletCount; i++) {
        //bullets[i]->erase();
        bullets[i]->update();
        if (!bullets[i]->isActive()) {
            delete bullets[i];
            bullets[i] = bullets[--bulletCount];
            bullets[bulletCount] = 0;
            i--;
        }
    }

    // Rendering
    unsigned char* real_video = (unsigned char*) video;
    video = backbuffer;

    clear_vga_screen(0); 
    grid.render();

    for (int i = 0; i < plantCount; i++)
        if (plants[i]) plants[i]->render();
    for (int i = 0; i < bulletCount; i++)
        if (bullets[i]) bullets[i]->render();
    for (int i = 0; i < zombieCount; i++)
        if (zombies[i]) zombies[i]->render();

    // compteurs
    draw_number(lastSeconds, 0, 2, 15, 2);
    draw_number(lastFps, 290, 2, 15, 2);

    unsigned char* src = backbuffer;
    unsigned char* dst = real_video;
    for (int i = 0; i < 320 * 200; i++)
        dst[i] = src[i];
    video = real_video;
}

void PlantsVsZombies::start() {
    grid.render();

    int px, py;
    grid.tileToPixel(0, 0, px, py);
    plants[0] = new Peashooter(px, py);
    plantCount = 1;
    plants[0]->render();

    int zx, zy;
    grid.tileToPixel(8, 0, zx, zy);
    zombies[0] = new Zombie(zx, zy);
    zombieCount = 1;
    zombies[0]->render();

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
            lastFps      = renderFrames;
            lastSeconds  = compt / 1000;
            renderFrames = 0;
            fpsTimer     = compt;
        }
    }
}
