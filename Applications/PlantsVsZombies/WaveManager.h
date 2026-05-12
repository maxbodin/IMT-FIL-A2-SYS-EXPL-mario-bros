#ifndef WAVE_MANAGER_H
#define WAVE_MANAGER_H

#include <Applications/PlantsVsZombies/Zombie.h>
#include <Applications/PlantsVsZombies/Grid.h>

#define BASE_ZOMBIES         1
#define ZOMBIES_PER_WAVE     1
#define MAX_WAVE             10   // waves loop after this
#define WAVE_PAUSE       12000   // ticks between waves (12 s)
#define SPAWN_INTERVAL    3000   // ticks between zombie spawns within a wave

class WaveManager {
public:
    WaveManager();

    /* Call every game tick. Returns a newly spawned Zombie,
       or 0 if no spawn this tick. Caller owns the pointer. */
    Zombie* update();

    int  getWave()      const { return wave; }
    bool isInPause()    const { return pauseUntil > 0; }

private:
    int wave;
    int remaining;
    int nextSpawnTick;
    int pauseUntil;

    int zombiesForWave(int w) const;
    int randomLane();
};

#endif
