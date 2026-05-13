#include <Applications/PlantsVsZombies/Chomper.h>
#include <Applications/PlantsVsZombies/Grid.h>
#include <Applications/PlantsVsZombies/sprites/plants/chomper_idle_sprite.h>
#include <Applications/PlantsVsZombies/sprites/plants/chomper_attacking_sprite.h>
#include <Applications/PlantsVsZombies/sprites/plants/chomper_chewing_sprite.h>
#include <vga/vga.h>

extern volatile int compt;

Chomper::Chomper(int x, int y)
    : Peashooter(x, y), chomperState(CHOMP_IDLE),
      attackFrame(0), attackAnimTick(0),
      chewFrame(0), chewAnimTick(0), chewEndTick(0)
{
    hp    = 300;
    maxHp = 300;
}

void Chomper::update() {
    if (state == DYING) { state = DEAD; return; }
    if (state == DEAD) return;

    switch (chomperState) {
        case CHOMP_IDLE:
            if (++animTick >= ANIM_SPEED) {
                animTick = 0;
                frame = (frame + 1) % CHOMPER_IDLE_FRAMES;
            }
            break;

        case CHOMP_ATTACKING:
            if (++attackAnimTick >= ATTACK_ANIM_SPEED) {
                attackAnimTick = 0;
                attackFrame++;
                if (attackFrame >= CHOMPER_ATTACKING_FRAMES) {
                    chomperState = CHOMP_CHEWING;
                    chewFrame = 0;
                    chewAnimTick = 0;
                    chewEndTick = compt + CHEW_TIME;
                }
            }
            break;

        case CHOMP_CHEWING:
            if (++chewAnimTick >= CHEW_ANIM_SPEED) {
                chewAnimTick = 0;
                chewFrame = (chewFrame + 1) % CHOMPER_CHEWING_FRAMES;
            }
            if (compt >= chewEndTick) {
                chomperState = CHOMP_IDLE;
                frame = 0;
                animTick = 0;
            }
            break;
    }
}

void Chomper::render() {
    if (state == DEAD) return;

    /* All chomper sprites are taller than TILE_SIZE, so we offset them
       upward so the sprite bottom aligns with the tile bottom. */
    switch (chomperState) {
        case CHOMP_IDLE: {
            int oy = y - (CHOMPER_IDLE_HEIGHT - Grid::TILE_SIZE);
            draw_sprite(chomper_idle_frames[frame],
                        CHOMPER_IDLE_WIDTH, CHOMPER_IDLE_HEIGHT, x, oy);
            renderHpBar(CHOMPER_IDLE_WIDTH / 2, CHOMPER_IDLE_HEIGHT + (y - oy));
            break;
        }

        case CHOMP_ATTACKING: {
            int oy = y - (CHOMPER_ATTACKING_HEIGHT - Grid::TILE_SIZE);
            draw_sprite(chomper_attacking_frames[attackFrame],
                        CHOMPER_ATTACKING_WIDTH, CHOMPER_ATTACKING_HEIGHT, x, oy);
            renderHpBar(CHOMPER_ATTACKING_WIDTH / 2, CHOMPER_ATTACKING_HEIGHT + (y - oy));
            break;
        }

        case CHOMP_CHEWING: {
            int oy = y - (CHOMPER_CHEWING_HEIGHT - Grid::TILE_SIZE);
            draw_sprite(chomper_chewing_frames[chewFrame],
                        CHOMPER_CHEWING_WIDTH, CHOMPER_CHEWING_HEIGHT, x, oy);
            renderHpBar(CHOMPER_CHEWING_WIDTH / 2, CHOMPER_CHEWING_HEIGHT + (y - oy));
            break;
        }
    }
}

bool Chomper::canShoot() const { return false; }
BulletType Chomper::getBulletType() const { return BULLET_PEASHOOTER; }
PlantType Chomper::getPlantType() const { return PLANT_CHOMPER; }

ChomperState Chomper::getChomperState() const { return chomperState; }

void Chomper::startAttack() {
    if (chomperState != CHOMP_IDLE) return;
    chomperState = CHOMP_ATTACKING;
    attackFrame = 0;
    attackAnimTick = 0;
}

bool Chomper::isChewing() const { return chomperState == CHOMP_CHEWING; }

int Chomper::getWidth() const { return CHOMPER_IDLE_WIDTH; }
int Chomper::getHeight() const { return CHOMPER_IDLE_HEIGHT; }
