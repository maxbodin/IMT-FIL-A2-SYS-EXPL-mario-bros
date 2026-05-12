#ifndef BULLET_POOL_H
#define BULLET_POOL_H

#include <Applications/PlantsVsZombies/Bullet.h>
#include <sextant/sync/Spinlock.h>

#define BULLET_POOL_SIZE 90

class BulletPool {
public:
    BulletPool();

    /* Acquires an inactive slot, initialises it, returns a pointer.
       Returns 0 if the pool is full.  Thread-safe (Spinlock).       */
    Bullet* acquire(int x, int y, BulletType type);

    /* Marks a bullet inactive.  Thread-safe (Spinlock). */
    void release(Bullet* b);

    /* Direct slot access for iteration (0 ≤ i < SIZE). */
    Bullet* get(int i);

    static const int SIZE = BULLET_POOL_SIZE;

private:
    Bullet pool[BULLET_POOL_SIZE];
    Spinlock lock;
};

#endif
