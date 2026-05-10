#include <sextant/sync/Semaphore.h>

/* [DANGER] P() est une attente active (busy-wait).
   Sur Sextant mono-core, cela ne fonctionne que si le producteur
   s'exécute en IRQ (qui préempte la boucle d'attente).
   Ne PAS appeler P() depuis une IRQ : deadlock garanti.           */
void Semaphore::P() {
    while (true) {
        lock.acquire();
        if (value > 0) {
            value--;
            lock.release();
            return;
        }
        lock.release();
    }
}

/* [EXPLICATION] V() peut être appelé depuis une IRQ (contexte IF=0).
   Le spinlock protège l'incrément contre un tryP() concurrent.
   Sur mono-core, un tryP() depuis le main loop ne peut pas s'exécuter
   en même temps qu'une IRQ — sauf si le main loop tient déjà le lock
   au moment de l'IRQ (d'où le disable_IRQs dans pop()).             */
void Semaphore::V() {
    lock.acquire();
    value++;
    lock.release();
}

/* [EXPLICATION] tryP() non-bloquant : renvoie true si un token a été
   consommé, false si la file était vide.
   [DANGER] Doit être appelé avec IF=0 (IRQ désactivées) pour éviter
   qu'une IRQ appelle V() pendant que ce lock est tenu.              */
bool Semaphore::tryP() {
    lock.acquire();
    if (value > 0) {
        value--;
        lock.release();
        return true;
    }
    lock.release();
    return false;
}
