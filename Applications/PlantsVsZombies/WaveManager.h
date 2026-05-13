#ifndef WAVE_MANAGER_H
#define WAVE_MANAGER_H

#include <Applications/PlantsVsZombies/Zombie.h>
#include <Applications/PlantsVsZombies/Grid.h>

#define BASE_ZOMBIES         2
#define ZOMBIES_PER_WAVE     3
#define MAX_WAVE             10   // waves loop after this
#define WAVE_PAUSE       12000   // ticks between waves (12 s)
#define SPAWN_INTERVAL    3000   // ticks between zombie spawns within a wave

/* Wave start text display durations (ticks). */
#define START_TEXT_DURATION 800   // each text shown for this long
#define START_TEXT_TOTAL   3200   // 4 texts × 800 ticks (Wave N + Ready + Set + Plant!)

class WaveManager {
public:
    WaveManager();

    /* Call every game tick. Returns a newly spawned Zombie,
       or 0 if no spawn this tick. Caller owns the pointer. */
    Zombie* update();

    /* Must be called each tick so WaveManager knows when all zombies are dead. */
    void setZombieCount(int count) { currentZombieCount = count; }

    int  getWave()      const { return wave; }
    bool isInPause()    const { return pauseUntil > 0; }
    void reset();

    /* Wave start text: returns 0=none, 1=Wave N, 2=Ready, 3=Set, 4=Plant! */
    int  getStartTextPhase() const;
    void renderStartText();

private:
    int wave;
    int remaining;
    int nextSpawnTick;
    int pauseUntil;

    int zombiesForWave(int w) const;
    int randomLane();

    int startTextTick;       // when the start text sequence began
    int currentZombieCount;  // live zombie count (set by game loop)
    bool wavePendingClear;   // waiting for all zombies to die before next wave
};

#endif
