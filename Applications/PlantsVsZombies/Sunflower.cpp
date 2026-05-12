#include <Applications/PlantsVsZombies/Sunflower.h>
#include <Applications/PlantsVsZombies/sprites/sunflower_sprite.h>
#include <vga/vga.h>

extern volatile int compt;

static unsigned int sf_rng = 77777;
static unsigned int sf_lcg() {
    sf_rng = sf_rng * 1103515245 + 12345;
    return (sf_rng >> 16) & 0x7FFF;
}

Sunflower::Sunflower(int x, int y)
    : Peashooter(x, y), nextSunTick(0)
{
    hp = 200;
    maxHp = 200;
    sf_rng ^= (unsigned int)compt;
    nextSunTick = compt + randomSunDelay();
}

int Sunflower::randomSunDelay() {
    sf_rng ^= (unsigned int)compt;
    return SUN_MIN_DELAY + (int)(sf_lcg() % (SUN_MAX_DELAY - SUN_MIN_DELAY + 1));
}

void Sunflower::update() {
    if (state == DYING) {
        state = DEAD;
        return;
    }
    if (state == DEAD) return;

    if (++animTick >= ANIM_SPEED) {
        animTick = 0;
        frame = (frame + 1) % SUNFLOWER_FRAMES;
    }
}

void Sunflower::render() {
    if (state == DEAD) return;
    draw_sprite(sunflower_frames[frame], SUNFLOWER_WIDTH, SUNFLOWER_HEIGHT, x, y);
    renderHpBar(SUNFLOWER_WIDTH / 2, SUNFLOWER_HEIGHT);
}

bool Sunflower::canShoot() const { return false; }
Bullet* Sunflower::createBullet(int, int) { return 0; }

bool Sunflower::hasSunReady() const {
    return state != DYING && state != DEAD && compt >= nextSunTick;
}

void Sunflower::resetSunTimer() {
    nextSunTick = compt + randomSunDelay();
}

int Sunflower::getWidth()  const { return SUNFLOWER_WIDTH; }
int Sunflower::getHeight() const { return SUNFLOWER_HEIGHT; }
