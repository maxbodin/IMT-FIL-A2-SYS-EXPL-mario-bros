#include <Applications/PlantsVsZombies/PotatoMine.h>
#include <Applications/PlantsVsZombies/sprites/potato_mine_sprite.h>
#include <Applications/PlantsVsZombies/sprites/potato_mine_explosion_sprite.h>
#include <vga/vga.h>

extern volatile int compt;

PotatoMine::PotatoMine(int x, int y)
    : Peashooter(x, y), exploding(false), explosionDone(false),
      explosionStartTick(0), explosionFrame(0), explosionAnimTick(0)
{
    hp    = 100;
    maxHp = 100;
}

void PotatoMine::update() {
    if (state == DYING) { state = DEAD; return; }
    if (state == DEAD)  return;

    if (exploding) {
        if (compt - explosionStartTick >= EXPLOSION_DURATION) {
            explosionDone = true;
            state = DYING;
            return;
        }
        if (++explosionAnimTick >= 6) {
            explosionAnimTick = 0;
            if (explosionFrame < POTATO_MINE_EXPLOSION_FRAMES - 1)
                explosionFrame++;
        }
    } else {
        if (++animTick >= ANIM_SPEED) {
            animTick = 0;
            frame = (frame + 1) % POTATO_MINE_FRAMES;
        }
    }
}

void PotatoMine::render() {
    if (state == DEAD) return;
    if (exploding) {
        /* Center explosion sprite on the mine position. */
        int ex = x - (POTATO_MINE_EXPLOSION_WIDTH  - POTATO_MINE_WIDTH)  / 2;
        int ey = y - (POTATO_MINE_EXPLOSION_HEIGHT - POTATO_MINE_HEIGHT) / 2;
        draw_sprite(potato_mine_explosion_frames[explosionFrame],
                    POTATO_MINE_EXPLOSION_WIDTH, POTATO_MINE_EXPLOSION_HEIGHT,
                    ex, ey);
    } else {
        draw_sprite(potato_mine_frames[frame],
                    POTATO_MINE_WIDTH, POTATO_MINE_HEIGHT, x, y);
        renderHpBar(POTATO_MINE_WIDTH / 2, POTATO_MINE_HEIGHT);
    }
}

void PotatoMine::explode() {
    if (!exploding && state != DEAD && state != DYING) {
        exploding = true;
        explosionStartTick = compt;
        explosionFrame = 0;
        explosionAnimTick = 0;
    }
}

bool PotatoMine::canShoot()       const { return false; }
BulletType PotatoMine::getBulletType() const { return BULLET_PEASHOOTER; }
PlantType PotatoMine::getPlantType() const { return PLANT_POTATO_MINE; }
bool PotatoMine::hasSunReady()    const { return false; }
void PotatoMine::resetSunTimer()        {}

int PotatoMine::getWidth()  const { return POTATO_MINE_WIDTH; }
int PotatoMine::getHeight() const { return POTATO_MINE_HEIGHT; }
