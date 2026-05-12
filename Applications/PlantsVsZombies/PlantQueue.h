#ifndef PLANT_QUEUE_H
#define PLANT_QUEUE_H

#include <sextant/sync/Spinlock.h>
#include <sextant/sync/Semaphore.h>
#include <Applications/PlantsVsZombies/PlantType.h>

/* Nombre de types de plantes disponibles dans le roster. */
#define ROSTER_SIZE       3
/* Nombre maximum de plantes en file d'attente par joueur. */
#define QUEUE_CAPACITY    3

/* Délais min/max entre chaque apparition
   d'une plante dans la file.  Un tirage pseudo-aléatoire
   choisit une valeur dans [MIN, MAX].                              */
#define QUEUE_SPAWN_MIN   2000   // 2 s
#define QUEUE_SPAWN_MAX   5000   // 5 s

/* Durée du flash rouge quand la file est vide (en ticks). */
#define QUEUE_EMPTY_FLASH 500

/* [EXPLICATION] PlantQueue — file de plantes par joueur.
   Architecture producteur / consommateur protégée par un Spinlock :

   - Producteur : update() appelé depuis la boucle de jeu.
     Après un délai aléatoire, pousse une plante aléatoire
     dans la file (si < QUEUE_CAPACITY).
   - Consommateur : tryTake() appelé quand le joueur place une plante.
     Retire et renvoie la plante sélectionnée par le curseur roster.

   Le Spinlock protège le tableau slots[] et slotCount contre la
   réentrance (même si, sur Sextant mono-thread sans preemption,
   les accès sont déjà séquentiels).

   Le Semaphore count trace le nombre de plantes disponibles :
   - V() quand une plante est ajoutée à la file,
   - tryP() quand le joueur tente de prendre une plante.             */

class PlantQueue {
public:
    PlantQueue();

    /* Met à jour la file : génère une plante après le délai. */
    void update();

    /* Tente de retirer la plante à l'index sélectionné (rosterCursor).
       Retourne true et écrit le type dans *out si succès.
       Retourne false si la file est vide (et déclenche le flash).    */
    bool tryTake(int index, PlantType& out);

    /* Pré-remplit la file avec n plantes aléatoires (appel au start). */
    void seed(int n);

    /* Accesseurs pour le rendu UI. */
    int        getCount()            const { return slotCount; }
    PlantType  getSlot(int i)        const { return slots[i]; }
    bool       isFlashing()          const;
    void       triggerFlash();
    int        getRosterCursor()     const { return rosterCursor; }
    void       moveRosterLeft();
    void       moveRosterRight();

    /* Coût en soleils du type de plante donné. */
    static int costOf(PlantType type);

private:
    PlantType  slots[QUEUE_CAPACITY];
    int        slotCount;
    int        nextSpawnTick;
    int        flashEndTick;
    int        rosterCursor;

    Spinlock   lock;
    Semaphore  available;

    PlantType  randomPlantType();
    int        randomDelay();
    void       pushSlot(PlantType type);
};

#endif
