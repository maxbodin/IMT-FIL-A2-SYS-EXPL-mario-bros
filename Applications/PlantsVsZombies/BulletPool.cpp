#include <Applications/PlantsVsZombies/BulletPool.h>

BulletPool::BulletPool() {}

Bullet* BulletPool::acquire(int x, int y, BulletType type) {
    lock.acquire();
    for (int i = 0; i < BULLET_POOL_SIZE; i++) {
        if (!pool[i].isActive()) {
            pool[i].init(x, y, type);
            lock.release();
            return &pool[i];
        }
    }
    lock.release();
    return 0;
}

void BulletPool::release(Bullet* b) {
    lock.acquire();
    if (b) b->deactivate();
    lock.release();
}

Bullet* BulletPool::get(int i) {
    if (i < 0 || i >= BULLET_POOL_SIZE) return 0;
    return &pool[i];
}
