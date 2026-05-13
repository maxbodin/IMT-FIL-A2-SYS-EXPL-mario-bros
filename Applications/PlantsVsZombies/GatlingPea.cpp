#include <Applications/PlantsVsZombies/GatlingPea.h>
#include <Applications/PlantsVsZombies/sprites/plants/gatlingpea_idle_sprite.h>
#include <Applications/PlantsVsZombies/sprites/plants/gatlingpea_shooting_sprite.h>
#include <vga/vga.h>

GatlingPea::GatlingPea(int x, int y)
    : Peashooter(x, y), shooting(false),
      shootFrame(0), shootAnimTick(0), shootHoldTicks(0)
{
    hp    = 300;
    maxHp = 300;
    cooldown = SHOOT_DELAY;
}

void GatlingPea::update() {
    if (state == DYING) { state = DEAD; return; }
    if (state == DEAD) return;

    if (cooldown > 0) cooldown--;

    if (shooting) {
        if (++shootAnimTick >= SHOOT_ANIM_SPEED) {
            shootAnimTick = 0;
            shootFrame = (shootFrame + 1) % GATLINGPEA_SHOOTING_FRAMES;
        }
        shootHoldTicks--;
        if (shootHoldTicks <= 0) {
            shooting = false;
            frame = 0;
            animTick = 0;
        }
    } else {
        if (++animTick >= ANIM_SPEED) {
            animTick = 0;
            frame = (frame + 1) % GATLINGPEA_IDLE_FRAMES;
        }
    }
}

void GatlingPea::render() {
    if (state == DEAD) return;

    if (shooting) {
        draw_sprite(gatlingpea_shooting_frames[shootFrame],
                    GATLINGPEA_SHOOTING_WIDTH, GATLINGPEA_SHOOTING_HEIGHT, x, y);
        renderHpBar(GATLINGPEA_SHOOTING_WIDTH / 2, GATLINGPEA_SHOOTING_HEIGHT);
    } else {
        draw_sprite(gatlingpea_idle_frames[frame],
                    GATLINGPEA_IDLE_WIDTH, GATLINGPEA_IDLE_HEIGHT, x, y);
        renderHpBar(GATLINGPEA_IDLE_WIDTH / 2, GATLINGPEA_IDLE_HEIGHT);
    }
}

bool GatlingPea::canShoot() const {
    return state == ALIVE && cooldown == 0;
}

void GatlingPea::resetCooldown() {
    cooldown = SHOOT_DELAY;
    shooting = true;
    shootFrame = 0;
    shootAnimTick = 0;
    shootHoldTicks = SHOOT_ANIM_HOLD;
}

BulletType GatlingPea::getBulletType() const { return BULLET_PEASHOOTER; }
PlantType GatlingPea::getPlantType() const { return PLANT_GATLING_PEA; }

int GatlingPea::getWidth() const { return GATLINGPEA_IDLE_WIDTH; }
int GatlingPea::getHeight() const { return GATLINGPEA_IDLE_HEIGHT; }
