#include <Applications/PlantsVsZombies/WallNut.h>
#include <Applications/PlantsVsZombies/sprites/plants/wallnut_sprite.h>
#include <Applications/PlantsVsZombies/sprites/plants/wallnut_damaged_sprite.h>
#include <Applications/PlantsVsZombies/sprites/plants/wallnut_very_damaged_sprite.h>
#include <vga/vga.h>

WallNut::WallNut(int x, int y) : Peashooter(x, y) {
    hp    = WALLNUT_HP;
    maxHp = WALLNUT_HP;
}

void WallNut::update() {
    if (state == DYING) { state = DEAD; return; }
    if (state == DEAD) return;

    if (++animTick >= ANIM_SPEED) {
        animTick = 0;
        frame = (frame + 1) % WALLNUT_FRAMES;
    }
}

void WallNut::render() {
    if (state == DEAD) return;

    /* Pick sprite set based on HP percentage. */
    int hpPct = (hp * 100) / maxHp;
    const unsigned char* spriteData;

    if (hpPct > 66) {
        spriteData = wallnut_frames[frame % WALLNUT_FRAMES];
    } else if (hpPct > 33) {
        spriteData = wallnut_damaged_frames[frame % WALLNUT_DAMAGED_FRAMES];
    } else {
        spriteData = wallnut_very_damaged_frames[frame % WALLNUT_VERY_DAMAGED_FRAMES];
    }

    draw_sprite(spriteData, WALLNUT_WIDTH, WALLNUT_HEIGHT, x, y);
    renderHpBar(WALLNUT_WIDTH / 2, WALLNUT_HEIGHT);
}

bool WallNut::canShoot()       const { return false; }
BulletType WallNut::getBulletType() const { return BULLET_PEASHOOTER; }
PlantType WallNut::getPlantType() const { return PLANT_WALLNUT; }
bool WallNut::hasSunReady()    const { return false; }
void WallNut::resetSunTimer()        {}

int WallNut::getWidth()  const { return WALLNUT_WIDTH; }
int WallNut::getHeight() const { return WALLNUT_HEIGHT; }
