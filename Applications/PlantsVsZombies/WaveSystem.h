#ifndef WAVE_SYSTEM_H
#define WAVE_SYSTEM_H

class WaveSystem {
public:
    WaveSystem();

    void seed(unsigned int s);
    void update(int zombieCount);
    bool hasPendingSpawn() const;
    int  consumeSpawnRow();
    int  getWave() const;
    void draw() const;

private:
    static const int WAVE_DELAY = 5000;
    static const int GRID_ROWS  = 5;

    int          wave;
    int          zombiesToSpawn;
    int          spawnTimer;
    int          spawnInterval;
    unsigned int rng;
    bool         pendingSpawn;
    int          pendingRow;

    void setupWave();
};

#endif
