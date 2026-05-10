#ifndef SPINLOCK_H
#define SPINLOCK_H

/* [EXPLICATION] Spinlock basé sur l'instruction atomique XCHG (x86).
   XCHG verrouille le bus mémoire pendant l'échange, garantissant
   l'atomicité même en contexte multi-core ou face aux DMA.

   Sur Sextant (mono-core), le spinlock protège surtout contre la
   réentrance depuis une IRQ.

   [DANGER] Si le main loop acquiert un spinlock avec IF=1 et qu'une
   IRQ tente d'acquérir le même spinlock → deadlock.
   => Toujours désactiver les IRQ AVANT d'acquérir un spinlock dans
      le main loop quand le producteur est une IRQ.                   */

class Spinlock {
public:
    void acquire();
    void release();

private:
    /* [DANGER] volatile : interdit au compilateur de cacher locked
       dans un registre entre deux accès — essentiel pour que la boucle
       d'attente voie les écritures de release().                     */
    volatile int locked { 0 };
};

#endif
