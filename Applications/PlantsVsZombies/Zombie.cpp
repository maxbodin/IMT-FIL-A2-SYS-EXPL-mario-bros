#include <Applications/PlantsVsZombies/Zombie.h>
#include <Applications/PlantsVsZombies/sprites/zombie_walk_sprite.h>
#include <Applications/PlantsVsZombies/sprites/zombie_fight_sprite.h>
#include <vga/vga.h>

Zombie::Zombie(int x, int y)
    : Entity(x, y, HP), cooldown(HIT_DELAY), frame(0), animTick(0), slowTicks(0) {}

void Zombie::update() {
    if (state == DYING) {
        state = DEAD;
        return;
    }

    if (state == DEAD)  return;

    if (cooldown > 0)
        cooldown--;
    if (slowTicks > 0)
        slowTicks--;

    int speed = (slowTicks > 0) ? ANIM_SPEED * 2 : ANIM_SPEED;

    if (state == BLOCKED) {
        if (++animTick >= speed) {
            animTick = 0;
            frame = (frame + 1) % ZOMBIE_FIGHT_FRAMES;
        }
    } else if (++animTick >= speed) {
        animTick = 0;
        frame = (frame + 1) % ZOMBIE_WALK_FRAMES;
        if (frame == 1 || frame == 5) {
            x -= 5; 
        }
    }
}

void Zombie::render() {
    if (state == DEAD) return;
    if (state == BLOCKED) {
        draw_sprite(zombie_fight_frames[frame], ZOMBIE_FIGHT_WIDTH, ZOMBIE_FIGHT_HEIGHT, x, y);
    } else {
        draw_sprite(zombie_walk_frames[frame], ZOMBIE_WALK_WIDTH, ZOMBIE_WALK_HEIGHT, x, y);
    }
    renderHpBar(ZOMBIE_WALK_WIDTH / 2, ZOMBIE_WALK_HEIGHT);
}

bool Zombie::canHit() const {
    return state != DYING && state != DEAD && cooldown == 0;
}

int Zombie::getWidth()  const { return ZOMBIE_WALK_WIDTH; }
int Zombie::getHeight() const { return ZOMBIE_WALK_HEIGHT; }

void Zombie::resetCooldown() {
    cooldown = HIT_DELAY;
}

void Zombie::block() { 
    if (state == ALIVE) state = BLOCKED;
}

void Zombie::unblock() { 
    if (state == BLOCKED) state = ALIVE; 
}

bool Zombie::isBlocked() const { 
    return state == BLOCKED; 
}

void Zombie::applySlow(int duration) {
    slowTicks = duration;
}

bool Zombie::isSlowed() const {
    return slowTicks > 0;
}
