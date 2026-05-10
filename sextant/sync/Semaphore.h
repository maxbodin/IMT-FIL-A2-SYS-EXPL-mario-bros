#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <sextant/sync/Spinlock.h>

/* [EXPLICATION] Sémaphore de comptage (TP6.2.1).
   value représente le nombre de ressources/tokens disponibles.

   - V() : incrémente value (signal — produit un token).
     Peut être appelé depuis une IRQ (producteur).
   - P() : décrémente value ; spin si value == 0 (attente active).
     [DANGER] Ne pas appeler depuis une IRQ : deadlock si le producteur
     est lui-même une IRQ et que le spinlock interne est déjà pris.
   - tryP() : P() non-bloquant ; renvoie true si succès, false si vide.
     Utilisé par la boucle de jeu pour drainer la file sans bloquer.

   Le Spinlock interne protège les lectures/écritures de value contre
   les accès concurrents (TP6.3.1).

   [DANGER] tryP() et V() utilisent tous deux le spinlock interne.
   Si tryP() est appelé avec IF=1 et qu'une IRQ appelle V() pendant
   que le spinlock est tenu → deadlock sur mono-core.
   => Appeler tryP() avec IF=0 (désactiver les IRQ avant).            */

class Semaphore {
public:
    explicit Semaphore(int initial = 0) : value(initial) {}

    void P();
    void V();
    bool tryP();

private:
    volatile int value;
    Spinlock     lock;
};

#endif
