#include <Applications/PlantsVsZombies/WaveManager.h>
#include <Applications/PlantsVsZombies/sprites/zombie_walk_sprite.h>

extern volatile int compt;

/* PRNG — LCG, sufficient for gameplay randomness. */
static unsigned int wave_rng = 54321;
static unsigned int wave_lcg() {
    wave_rng = wave_rng * 1103515245 + 12345;
    return (wave_rng >> 16) & 0x7FFF;
}

WaveManager::WaveManager()
    : wave(0), remaining(0), nextSpawnTick(0), pauseUntil(0) {}

int WaveManager::zombiesForWave(int w) const {
    return BASE_ZOMBIES + (w - 1) * ZOMBIES_PER_WAVE;
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
    }

    /* Start next wave when current one is exhausted — loops after MAX_WAVE */
    if (remaining <= 0) {
        wave++;
        remaining     = zombiesForWave(((wave - 1) % MAX_WAVE) + 1);
        nextSpawnTick = compt;
    }

    if (compt < nextSpawnTick) return 0;

    int lane = randomLane();
    int zy   = Grid::OFFSET_Y + lane * Grid::TILE_SIZE;
    Zombie* z = new Zombie(320 - ZOMBIE_WALK_WIDTH, zy);

    remaining--;
    nextSpawnTick = compt + SPAWN_INTERVAL;

    /* Set pause before next wave once this one is fully spawned. */
    if (remaining <= 0)
        pauseUntil = compt + WAVE_PAUSE;

    return z;
}
