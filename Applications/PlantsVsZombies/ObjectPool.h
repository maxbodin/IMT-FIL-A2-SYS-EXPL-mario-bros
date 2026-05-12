#ifndef OBJECT_POOL_H
#define OBJECT_POOL_H

#include <sextant/sync/Spinlock.h>

/* Generic fixed-size object pool, Spinlock-protected.
   Contract — T must provide:
     - T()               default constructor (inactive state)
     - bool isActive() const
     - void deactivate()
   Caller is responsible for calling T-specific init() on
   the pointer returned by acquire().                         */
template<typename T, int SIZE>
class ObjectPool {
public:
    static const int CAPACITY = SIZE;

    ObjectPool() {}

    /* Finds the first inactive slot and returns a pointer to it.
       Returns 0 if the pool is full.  Thread-safe. */
    T* acquire() {
        lock.acquire();
        for (int i = 0; i < SIZE; i++) {
            if (!items[i].isActive()) {
                lock.release();
                return &items[i];
            }
        }
        lock.release();
        return 0;
    }

    /* Marks an item inactive.  Thread-safe. */
    void release(T* item) {
        lock.acquire();
        if (item) item->deactivate();
        lock.release();
    }

    /* Direct slot access for iteration (0 <= i < CAPACITY). */
    T* get(int i) {
        if (i < 0 || i >= SIZE) return 0;
        return &items[i];
    }

private:
    T items[SIZE];
    Spinlock lock;
};

#endif
