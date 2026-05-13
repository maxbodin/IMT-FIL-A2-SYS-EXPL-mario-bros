#include <Applications/PlantsVsZombies/WaveSystem.h>
#include <vga/vga.h>

static const unsigned char HUD_BG     = 1;
static const unsigned char HUD_BORDER = 3;
static const unsigned char HUD_TEXT   = 15;

// Paramètres par vague : zombies à spawner + intervalle entre chaque spawn
void WaveSystem::setupWave() {
    if (wave == 1) {
        zombiesToSpawn = 5;
        spawnInterval  = 700;
    } else {
        zombiesToSpawn = 5 + wave * 2; // vague 2 → 9, vague 3 → 11 …
        spawnInterval  = 300;          // quasi-simultané
    }
}

WaveSystem::WaveSystem()
    : wave(1), zombiesToSpawn(0), spawnTimer(0), spawnInterval(2000),
      rng(12345u), pendingSpawn(false), pendingRow(0) {
    setupWave();
}

void WaveSystem::seed(unsigned int s) { rng = s; }

void WaveSystem::update(int zombieCount) {
    if (spawnTimer > 0) spawnTimer--;

    if (!pendingSpawn && zombiesToSpawn > 0 && spawnTimer == 0) {
        rng          = rng * 1664525u + 1013904223u;
        pendingRow   = (int)((rng >> 16) % (unsigned int)GRID_ROWS);
        pendingSpawn = true;
        zombiesToSpawn--;
        if (zombiesToSpawn > 0) spawnTimer = spawnInterval;
    }

    if (!pendingSpawn && zombiesToSpawn == 0 && zombieCount == 0) {
        wave++;
        setupWave();
        spawnTimer = WAVE_DELAY;
    }
}

bool WaveSystem::hasPendingSpawn() const { return pendingSpawn; }

int WaveSystem::consumeSpawnRow() {
    pendingSpawn = false;
    return pendingRow;
}

int WaveSystem::getWave() const { return wave; }

void WaveSystem::draw() const {
    const int px = 254, py = 1, pw = 32, ph = 20;
    for (int row = 0; row < ph; row++)
        for (int col = 0; col < pw; col++)
            video[(py + row) * 320 + (px + col)] = HUD_BG;
    for (int col = 0; col < pw; col++) {
        video[py * 320 + (px + col)]            = HUD_BORDER;
        video[(py + ph - 1) * 320 + (px + col)] = HUD_BORDER;
    }
    draw_number(wave, px + 6, py + 6, HUD_TEXT, 2);
}
