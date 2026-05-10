#ifndef KEYBOARD_QUEUE_H
#define KEYBOARD_QUEUE_H

#include <sextant/sync/Semaphore.h>

#define KBD_QUEUE_CAP 32

/* [EXPLICATION] Événement clavier brut : scan code PS/2 + direction.
   pressed=true  → touche enfoncée (make code, c < 0x80).
   pressed=false → touche relâchée (break code, c >= 0x80).          */
struct KeyEvent {
    unsigned char scanCode;
    bool          pressed;
};

/* [EXPLICATION] File circulaire (ring buffer) — architecture producteur/consommateur.

   Producteur : handler_clavier() (IRQ1, IF=0)
     → push() : écrit dans buffer[tail], incrémente tail, appelle V().

   Consommateur : PlantsVsZombies::handleInput() (main loop, IF=1)
     → pop() : désactive les IRQ, appelle tryP(), lit buffer[head],
               incrémente head, réactive les IRQ.

   Le sémaphore count compte les événements disponibles.
   V() signale un nouvel événement ; tryP() en consomme un sans bloquer.

   [DANGER] Deadlock potentiel sans précaution :
     - main loop appelle tryP() → Spinlock interne acquis (IF=1)
     - IRQ se déclenche → V() tente le même Spinlock → spin infini.
   Solution : désactiver les IRQ dans pop() AVANT tryP().
   Ainsi tryP() s'exécute toujours avec IF=0 → pas d'IRQ concurrent.

   push() n'a pas besoin de protection : il s'exécute déjà avec IF=0
   (on est dans le handler IRQ1).                                      */

class KeyboardQueue {
public:
    void push(unsigned char scanCode, bool pressed);
    bool pop(KeyEvent& out);

private:
    KeyEvent     buffer[KBD_QUEUE_CAP];
    volatile int head { 0 };
    volatile int tail { 0 };
    Semaphore    count;
};

/* Instance globale : extern ici, définie dans KeyboardQueue.cpp.
   Accédée par handler_clavier (push) et PlantsVsZombies (pop).  */
extern KeyboardQueue keyboardQueue;

#endif
