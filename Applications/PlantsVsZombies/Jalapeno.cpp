#include <Applications/PlantsVsZombies/Jalapeno.h>
#include <Applications/PlantsVsZombies/sprites/jalapeno_sprite.h>
#include <Applications/PlantsVsZombies/sprites/jalapeno_fire_sprite.h>
#include <vga/vga.h>

extern volatile int compt;

static unsigned int jal_rng = 31337;
static unsigned int jal_lcg() {
    jal_rng = jal_rng * 1103515245 + 12345;
    return (jal_rng >> 16) & 0x7FFF;
}

Jalapeno::Jalapeno(int x, int y)
    : Peashooter(x, y), onFire(false), fireDone(false),
      fireStartTick(0), fireFrame(0), fireAnimTick(0)
{
    hp    = 100;
    maxHp = 100;
}

void Jalapeno::update() {
    if (state == DYING) { state = DEAD; return; }
    if (state == DEAD)  return;

    if (onFire) {
        if (compt - fireStartTick >= FIRE_DURATION) {
            fireDone = true;
            state = DYING;
            return;
        }
        if (++fireAnimTick >= 8) {
            fireAnimTick = 0;
            fireFrame = (fireFrame + 1) % JALAPENO_FIRE_FRAMES;
        }
    } else {
        if (++animTick >= ANIM_SPEED) {
            animTick = 0;
            frame = (frame + 1) % JALAPENO_FRAMES;
        }
    }
}

void Jalapeno::render() {
    if (state == DEAD) return;
    if (onFire) {
        draw_sprite(jalapeno_fire_frames[fireFrame],
                    JALAPENO_FIRE_WIDTH, JALAPENO_FIRE_HEIGHT,
                    x, y + JALAPENO_HEIGHT - JALAPENO_FIRE_HEIGHT);
    } else {
        draw_sprite(jalapeno_frames[frame],
                    JALAPENO_WIDTH, JALAPENO_HEIGHT, x, y);
    }
    renderHpBar(JALAPENO_WIDTH / 2, JALAPENO_HEIGHT);
}

void Jalapeno::ignite() {
    if (!onFire && state != DEAD && state != DYING) {
        onFire = true;
        fireStartTick = compt;
        fireFrame = 0;
        fireAnimTick = 0;
    }
}

int Jalapeno::getFireDamage() {
    jal_rng ^= (unsigned int)compt;
    return FIRE_DMG_MIN + (int)(jal_lcg() % (FIRE_DMG_MAX - FIRE_DMG_MIN + 1));
}

int Jalapeno::getFireEffectDuration() {
    jal_rng ^= (unsigned int)compt;
    return 800 + (int)(jal_lcg() % 1201); // 800-2000 ticks
}

bool Jalapeno::canShoot()       const { return false; }
BulletType Jalapeno::getBulletType() const { return BULLET_PEASHOOTER; }
PlantType Jalapeno::getPlantType() const { return PLANT_JALAPENO; }
bool Jalapeno::hasSunReady()    const { return false; }
void Jalapeno::resetSunTimer()        {}

int Jalapeno::getWidth()  const { return JALAPENO_WIDTH; }
int Jalapeno::getHeight() const { return JALAPENO_HEIGHT; }
