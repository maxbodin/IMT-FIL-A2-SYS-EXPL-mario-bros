#include <Applications/PlantsVsZombies/WaveManager.h>
#include <Applications/PlantsVsZombies/sprites/zombie_walk_sprite.h>
#include <Applications/PlantsVsZombies/sprites/start_ready_sprite.h>
#include <Applications/PlantsVsZombies/sprites/start_set_sprite.h>
#include <Applications/PlantsVsZombies/sprites/start_plant_sprite.h>
#include <vga/vga.h>

extern volatile int compt;

/* PRNG — LCG, sufficient for gameplay randomness. */
static unsigned int wave_rng = 54321;
static unsigned int wave_lcg() {
    wave_rng = wave_rng * 1103515245 + 12345;
    return (wave_rng >> 16) & 0x7FFF;
}

WaveManager::WaveManager()
    : wave(0), remaining(0), nextSpawnTick(0), pauseUntil(0), startTextTick(0),
      currentZombieCount(0), wavePendingClear(false) {}

/* Integer log2 approximation */
static int ilog2(int v) {
    int r = 0;
    while (v > 1) { v >>= 1; r++; }
    return r;
}

int WaveManager::zombiesForWave(int w) const {
    // Logarithmic scaling: grows fast early, plateaus later
    return BASE_ZOMBIES + ilog2(w) * ZOMBIES_PER_WAVE;
}

int WaveManager::randomLane() {
    wave_rng ^= (unsigned int)compt;
    return (int)(wave_lcg() % Grid::ROWS);
}

Zombie* WaveManager::update() {
    /* Inter-wave pause */
    if (pauseUntil > 0) {
        if (compt < pauseUntil) return 0;
        pauseUntil = 0;
        /* After pause expires, wait for all zombies to be killed */
        wavePendingClear = true;
    }

    /* Wait until all zombies from previous wave are dead */
    if (wavePendingClear) {
        if (currentZombieCount > 0) return 0;
        wavePendingClear = false;
    }

    /* Start next wave when current one is exhausted — loops after MAX_WAVE */
    if (remaining <= 0) {
        wave++;
        remaining     = zombiesForWave(((wave - 1) % MAX_WAVE) + 1);
        nextSpawnTick = compt + START_TEXT_TOTAL; // delay spawning until text finishes
        startTextTick = compt; // begin Ready/Set/Plant! sequence
    }

    if (compt < nextSpawnTick) return 0;

    int lane = randomLane();
    // Align zombie feet (bottom of sprite) with the middle of the tile lane
    int zy   = Grid::OFFSET_Y + lane * Grid::TILE_SIZE + Grid::TILE_SIZE / 2 - ZOMBIE_WALK_HEIGHT;
    Zombie* z = new Zombie(320 - ZOMBIE_WALK_WIDTH, zy);

    remaining--;
    nextSpawnTick = compt + SPAWN_INTERVAL;

    /* Set pause before next wave once this one is fully spawned. */
    if (remaining <= 0)
        pauseUntil = compt + WAVE_PAUSE;

    return z;
}

int WaveManager::getStartTextPhase() const {
    if (startTextTick == 0) return 0;
    int elapsed = compt - startTextTick;
    if (elapsed >= START_TEXT_TOTAL) return 0;
    return (elapsed / START_TEXT_DURATION) + 1; // 1=Wave N, 2=Ready, 3=Set, 4=Plant!
}

void WaveManager::renderStartText() {
    int phase = getStartTextPhase();
    if (phase == 0) return;

    /* Phase 1: "Wave N" — drawn with draw_text/draw_number */
    if (phase == 1) {
        const char* label = "wave ";
        // Compute total width: "wave " (5 chars) + digits of wave number
        int numDigits = 0;
        int tmp = wave;
        if (tmp == 0) numDigits = 1;
        else while (tmp > 0) { numDigits++; tmp /= 10; }
        int scale = 3;
        int charW = 4 * scale; // each char is 4px wide at scale
        int totalW = (5 + numDigits) * charW;
        int dx = (320 - totalW) / 2;
        int dy = (200 - 5 * scale) / 2; // 5px char height
        draw_text(label, dx, dy, 15, scale);
        draw_number(wave, dx + 5 * charW, dy, 15, scale);
        return;
    }

    const unsigned char* data;
    int srcW, srcH;

    switch (phase) {
        case 2:
            data = start_ready_sprite_data;
            srcW = START_READY_WIDTH;
            srcH = START_READY_HEIGHT;
            break;
        case 3:
            data = start_set_sprite_data;
            srcW = START_SET_WIDTH;
            srcH = START_SET_HEIGHT;
            break;
        case 4:
        default:
            data = start_plant_sprite_data;
            srcW = START_PLANT_WIDTH;
            srcH = START_PLANT_HEIGHT;
            break;
    }

    /* Draw centered on screen (320x200). Scale to fit nicely. */
    int dstW = srcW;
    int dstH = srcH;
    /* Cap to screen width with some margin */
    if (dstW > 280) {
        dstH = dstH * 280 / dstW;
        dstW = 280;
    }
    int dx = (320 - dstW) / 2;
    int dy = (200 - dstH) / 2;
    draw_sprite_scaled(data, srcW, srcH, dx, dy, dstW, dstH);
}
