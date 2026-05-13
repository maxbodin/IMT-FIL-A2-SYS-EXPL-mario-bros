#include <Applications/PlantsVsZombies/SnowPeashooter.h>
#include <Applications/PlantsVsZombies/sprites/plants/snow_peashooter_sprite.h>
#include <Applications/PlantsVsZombies/sprites/plants/snow_peashooter_shooting_sprite.h>
#include <vga/vga.h>

SnowPeashooter::SnowPeashooter(int x, int y) : Peashooter(x, y) {}

void SnowPeashooter::update() {
    if (state == DYING) { state = DEAD; return; }
    if (cooldown > 0) cooldown--;

    if (shooting) {
        if (++shootAnimTick >= 6) {
            shootAnimTick = 0;
            shootFrame = (shootFrame + 1) % SNOW_PEASHOOTER_SHOOTING_FRAMES;
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
            frame = (frame + 1) % SNOW_PEASHOOTER_FRAMES;
        }
    }
}

void SnowPeashooter::render() {
    if (getState() == DEAD) return;
    int dstW = 28, dstH = 28;
    if (shooting) {
        draw_sprite(snow_peashooter_shooting_frames[shootFrame],
                    SNOW_PEASHOOTER_SHOOTING_WIDTH, SNOW_PEASHOOTER_SHOOTING_HEIGHT, x, y);
        renderHpBar(SNOW_PEASHOOTER_SHOOTING_WIDTH / 2, SNOW_PEASHOOTER_SHOOTING_HEIGHT);
    } else {
        draw_sprite_scaled(snow_peashooter_frames[frame], SNOW_PEASHOOTER_WIDTH, SNOW_PEASHOOTER_HEIGHT,
                           x, y, dstW, dstH);
        renderHpBar(dstW / 2, dstH);
    }
}

BulletType SnowPeashooter::getBulletType() const {
    return BULLET_SNOW_PEA;
}

PlantType SnowPeashooter::getPlantType() const { return PLANT_SNOW_PEASHOOTER; }
