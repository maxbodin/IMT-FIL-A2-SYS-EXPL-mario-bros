#include <sextant/sync/Spinlock.h>

/* [EXPLICATION] Algorithme test-and-set via XCHG :
   on échange atomiquement la valeur 1 avec locked.
   - Si XCHG renvoie 0 : le spinlock était libre, on l'a acquis.
   - Si XCHG renvoie 1 : quelqu'un le tenait, on recommence (busy-wait).
   Le clobber "memory" force le compilateur à vider ses caches de registres
   autour de l'instruction — barrière mémoire implicite.                  */
void Spinlock::acquire() {
    int tmp;
    do {
        asm volatile (
            "movl $1, %0\n\t"
            "xchgl %0, %1"
            : "=r"(tmp), "+m"(locked)
            :
            : "memory"
        );
    } while (tmp != 0);
}

/* [EXPLICATION] release() : écriture directe de 0.
   Sur x86, un store 32 bits sur adresse alignée est atomique.
   volatile + "memory" assurent que le compilateur ne réordonne pas
   cette écriture après d'autres accès mémoire.                       */
void Spinlock::release() {
    asm volatile ("" ::: "memory");
    locked = 0;
}
