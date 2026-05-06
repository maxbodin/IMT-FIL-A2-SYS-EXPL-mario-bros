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

static bool aabb(int ax, int ay, int aw, int ah,
                 int bx, int by, int bw, int bh) {
    return ax < bx + bw && ax + aw > bx
        && ay < by + bh && ay + ah > by;
}

void PlantsVsZombies::update_screen() {

    // --- Plants: update + spawn bullets + remove dead ---
    for (int i = 0; i < plantCount; i++) {
        plants[i]->update();
        if (plants[i]->isDead()) {
            delete plants[i];
            plants[i] = plants[--plantCount];
            plants[plantCount] = 0;
            i--;
            continue;
        }
        if (plants[i]->canShoot() && bulletCount < MAX_BULLETS) {
            int bx = plants[i]->getX();
            int by = plants[i]->getY() + plants[i]->getHeight() / 4;
            bullets[bulletCount++] = new PeashooterBullet(bx, by);
            plants[i]->resetCooldown();
        }
    }

    // --- Zombies: block/unblock + damage plant + update + remove dead ---
    for (int i = 0; i < zombieCount; i++) {
        bool blocked = false;
        for (int p = 0; p < plantCount; p++) {
            int dx = zombies[i]->getX() - (plants[p]->getX() + plants[p]->getWidth());
            int dy = zombies[i]->getY() - plants[p]->getY();
            if (dy < 0) dy = -dy;
            if (dx >= 0 && dx < COLLISION_DISTANCE && dy < Grid::TILE_SIZE) {
                blocked = true;
                if (zombies[i]->canHit()) {
                    plants[p]->takeDamage(ZOMBIE_DAMAGE);
                    zombies[i]->resetCooldown();
                }
                break;
            }
        }
        blocked ? zombies[i]->block() : zombies[i]->unblock();
        zombies[i]->update();
        if (zombies[i]->isDead()) {
            delete zombies[i];
            zombies[i] = zombies[--zombieCount];
            zombies[zombieCount] = 0;
            i--;
        }
    }

    // --- Bullets: update + collision vs zombies + remove inactive ---
    for (int i = 0; i < bulletCount; i++) {
        bullets[i]->update();
        if (bullets[i]->isActive()
                && bullets[i]->getX() > bullets[i]->getSpawnX() + COLLISION_DISTANCE) {
            for (int z = 0; z < zombieCount; z++) {
                if (aabb(bullets[i]->getX(),  bullets[i]->getY(),
                         bullets[i]->getWidth(), bullets[i]->getHeight(),
                         zombies[z]->getX(),  zombies[z]->getY(),
                         zombies[z]->getWidth(), zombies[z]->getHeight())) {
                    zombies[z]->takeDamage(bullets[i]->getDamage());
                    bullets[i]->deactivate();
                    break;
                }
            }
        }
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
