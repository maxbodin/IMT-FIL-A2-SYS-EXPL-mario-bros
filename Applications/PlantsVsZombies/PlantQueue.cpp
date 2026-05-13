#include <Applications/PlantsVsZombies/PlantQueue.h>
#include <Applications/PlantsVsZombies/PlantsVsZombies.h>
#include <Applications/PlantsVsZombies/Peashooter.h>
#include <Applications/PlantsVsZombies/SnowPeashooter.h>
#include <Applications/PlantsVsZombies/Sunflower.h>
#include <Applications/PlantsVsZombies/Jalapeno.h>
#include <Applications/PlantsVsZombies/PotatoMine.h>
#include <Applications/PlantsVsZombies/WallNut.h>
#include <Applications/PlantsVsZombies/Chomper.h>
#include <Applications/PlantsVsZombies/GatlingPea.h>
#include <sextant/interruptions/irq.h>

extern volatile int compt;

/* PRNG minimaliste (Linear Congruential Generator).
   Suffisant pour un jeu — pas de sécurité requise.    */
static unsigned int rng_state = 12345;
static unsigned int lcg_next() {
    rng_state = rng_state * 1103515245 + 12345;
    return (rng_state >> 16) & 0x7FFF;
}

/* ------------------------------------------------------------------ */

PlantQueue::PlantQueue()
    : slotCount(0), nextSpawnTick(0), flashEndTick(0),
      rosterCursor(0), available(0)
{
    for (int i = 0; i < QUEUE_CAPACITY; i++)
        slots[i] = PLANT_PEASHOOTER;
}

PlantType PlantQueue::randomPlantType() {
    /* Mélange l'état PRNG avec compt pour plus de variété.  */
    rng_state ^= (unsigned int)compt;
    int roll = lcg_next() % ROSTER_SIZE;
    switch (roll) {
        case 0: return PLANT_PEASHOOTER;
        case 1: return PLANT_SNOW_PEASHOOTER;
        case 2: return PLANT_SUNFLOWER;
        case 3: return PLANT_JALAPENO;
        case 4: return PLANT_POTATO_MINE;
        case 5: return PLANT_WALLNUT;
        case 6: return PLANT_CHOMPER;
        default: return PLANT_GATLING_PEA;
    }
}

int PlantQueue::randomDelay() {
    return QUEUE_SPAWN_MIN
         + (int)(lcg_next() % (QUEUE_SPAWN_MAX - QUEUE_SPAWN_MIN + 1));
}

void PlantQueue::seed(int n) {
    for (int i = 0; i < n && slotCount < QUEUE_CAPACITY; i++)
        pushSlot(randomPlantType());
    nextSpawnTick = compt + randomDelay();
}

void PlantQueue::pushSlot(PlantType type) {
    lock.acquire();
    if (slotCount < QUEUE_CAPACITY) {
        slots[slotCount++] = type;
        available.V();
    }
    lock.release();
}

void PlantQueue::update() {
    if (slotCount >= QUEUE_CAPACITY) {
        nextSpawnTick = compt + randomDelay();
        return;
    }
    if (compt >= nextSpawnTick) {
        pushSlot(randomPlantType());
        nextSpawnTick = compt + randomDelay();
    }
}

bool PlantQueue::tryTake(int index, PlantType& out) {
    ui32_t flags;
    disable_IRQs(flags);
    bool ok = available.tryP();
    restore_IRQs(flags);

    if (!ok) {
        flashEndTick = compt + QUEUE_EMPTY_FLASH;
        return false;
    }

    lock.acquire();
    if (index < 0 || index >= slotCount) {
        /* Index invalide : remettre le token et annuler. */
        available.V();
        lock.release();
        return false;
    }
    out = slots[index];
    /* Décale les éléments suivants vers la gauche. */
    for (int i = index; i < slotCount - 1; i++)
        slots[i] = slots[i + 1];
    slotCount--;
    /* Clamp le curseur roster pour qu'il reste sur un slot valide. */
    if (slotCount > 0) {
        if (rosterCursor >= slotCount)
            rosterCursor = slotCount - 1;
    } else {
        rosterCursor = 0;
    }
    lock.release();
    return true;
}

bool PlantQueue::isFlashing() const {
    return compt < flashEndTick;
}

void PlantQueue::triggerFlash() {
    flashEndTick = compt + QUEUE_EMPTY_FLASH;
}

void PlantQueue::moveRosterLeft() {
    if (slotCount <= 0) 
        return;

    rosterCursor--;

    if (rosterCursor < 0) 
        rosterCursor = slotCount - 1;
}

void PlantQueue::moveRosterRight() {
    if (slotCount <= 0) 
        return;

    rosterCursor++;

    if (rosterCursor >= slotCount) 
        rosterCursor = 0;
}

int PlantQueue::costOf(PlantType type) {
    switch (type) {
        case PLANT_SNOW_PEASHOOTER: return SnowPeashooter::COST;
        case PLANT_SUNFLOWER:       return Sunflower::COST;
        case PLANT_JALAPENO:        return Jalapeno::COST;
        case PLANT_POTATO_MINE:     return PotatoMine::COST;
        case PLANT_WALLNUT:         return WallNut::COST;
        case PLANT_CHOMPER:         return Chomper::COST;
        case PLANT_GATLING_PEA:     return GatlingPea::COST;
        case PLANT_PEASHOOTER:
        default:                    return Peashooter::COST;
    }
}
