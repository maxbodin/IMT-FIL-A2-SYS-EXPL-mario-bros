#include <sextant/sync/KeyboardQueue.h>
#include <sextant/interruptions/irq.h>

KeyboardQueue keyboardQueue;

/* [EXPLICATION] push() est appelé depuis handler_clavier() (IRQ1, IF=0).
   Les interruptions sont déjà masquées → accès exclusif garanti.
   On écrit buffer[tail] PUIS on incrémente tail PUIS on appelle V(),
   dans cet ordre strict : le consommateur ne doit voir le token qu'après
   que la donnée soit entièrement écrite.                               */
void KeyboardQueue::push(unsigned char scanCode, bool pressed) {
    int next = (tail + 1) % KBD_QUEUE_CAP;
    if (next == head)
        return; /* [DANGER] file pleine : on drop l'événement plutôt que de bloquer en IRQ */

    buffer[tail].scanCode = scanCode;
    buffer[tail].pressed  = pressed;
    tail = next;
    count.V();
}

/* [EXPLICATION] pop() est appelé depuis la boucle de jeu (main loop, IF=1).
   Séquence :
     1. disable_IRQs → IF=0 : aucune IRQ ne peut appeler push()/V() pendant tryP().
     2. tryP() tente de consommer un token (spinlock interne) — sans risque de deadlock.
     3. restore_IRQs → IF=1 : les IRQ reprennent normalement.
     4. Si succès : lire buffer[head] et avancer head (seul le consommateur le fait).

   [DANGER] Sans disable_IRQs en étape 1 :
     - tryP() acquiert le spinlock (IF=1)
     - IRQ clavier se déclenche → V() tente le même spinlock → spin infini → deadlock. */
bool KeyboardQueue::pop(KeyEvent& out) {
    ui32_t flags;
    disable_IRQs(flags);
    bool ok = count.tryP();
    restore_IRQs(flags);

    if (!ok) return false;

    /* Après tryP() réussi, head < tail (modulo KBD_QUEUE_CAP), donc buffer[head]
       est valide.  Seul pop() modifie head → pas de concurrent.                   */
    out  = buffer[head];
    head = (head + 1) % KBD_QUEUE_CAP;
    return true;
}
