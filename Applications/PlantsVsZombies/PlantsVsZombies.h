#ifndef PLANTSVSZOMBIES_H
#define PLANTSVSZOMBIES_H

#include <Applications/PlantsVsZombies/Grid.h>
#include <Applications/PlantsVsZombies/Peashooter.h>
#include <Applications/PlantsVsZombies/SnowPeashooter.h>
#include <Applications/PlantsVsZombies/Sunflower.h>
#include <Applications/PlantsVsZombies/Jalapeno.h>
#include <Applications/PlantsVsZombies/PotatoMine.h>
#include <Applications/PlantsVsZombies/WallNut.h>
#include <Applications/PlantsVsZombies/Bullet.h>
#include <Applications/PlantsVsZombies/Zombie.h>
#include <Applications/PlantsVsZombies/DmgIndicator.h>
#include <Applications/PlantsVsZombies/ObjectPool.h>
#include <Applications/PlantsVsZombies/Sun.h>
#include <Applications/PlantsVsZombies/PlantQueue.h>
#include <Applications/PlantsVsZombies/WaveManager.h>
#include <drivers/Clavier.h>
#include <sextant/sync/Semaphore.h>

#define MAX_PLANTS  45
#define MAX_ZOMBIES 20

#define BULLET_POOL_SIZE 90

#define COLLISION_DISTANCE 5
#define ZOMBIE_DAMAGE      25

#define SUN_INITIAL           150
#define SUN_TICK_INTERVAL    5000  // ticks entre chaque gain automatique (5 s à 1000 Hz)
#define SUN_TICK_AMOUNT        25  // soleils gagnés à chaque intervalle
#define SUN_DISPLAY_DURATION 2000  // durée d'affichage du "+Y" en ticks (2 s)
#define SUN_COLLECT_DISPLAY  1500  // durée d'affichage du "+X" après collecte

class PlantsVsZombies {
public:
    PlantsVsZombies();

    void init(Ecran* e, Clavier* c);
    void start();

    bool canAfford(int cost) const;
    void spendSuns(int cost);

    /* Points d'entrée des threads de jeu.
       Chaque méthode est une boucle infinie exécutée par son thread dédié. */
    void runLogic();   /* Thread logique : mise à jour de l'état du jeu   */
    void runRender();  /* Thread rendu  : composition backbuffer + blit VGA */

private:
    Grid        grid;
    Peashooter* plants[MAX_PLANTS];
    int         plantCount;
    ObjectPool<Bullet, BULLET_POOL_SIZE>         bulletPool;
    ObjectPool<DmgIndicator, MAX_DMG_INDICATORS> dmgPool;
    Zombie*     zombies[MAX_ZOMBIES];
    int         zombieCount;

    Ecran*         ecran;
    Clavier*       clavier;
    unsigned char* backbuffer;

    int  suns                 { SUN_INITIAL };
    int  lastSunTick          { 0 };
    int  sunGainDisplayEnd    { 0 };
    int  lastFps              { 0 };
    int  lastSeconds          { 0 };
    int  sunFlashEndTick      { 0 };
    int  sunCollectDisplayEnd { 0 };
    int  lastSunCollected     { 0 };
    int  lives                { 3 };
    bool gameOver              { false };

    Sun* suns_on_ground[MAX_SUNS];
    int  sunOnGroundCount     { 0 };

    WaveManager waveManager;
    PlantQueue  queue1;
    PlantQueue  queue2;

    int cursorCol  { 0 };
    int cursorRow  { 0 };
    int cursorCol2 { Grid::COLS - 1 };
    int cursorRow2 { Grid::ROWS - 1 };

    /* [SYNC] Sémaphore de synchronisation logique → rendu.
       Le thread logique fait V() après chaque mise à jour de l'état du jeu.
       Le thread rendu fait tryP() (non bloquant) et cède le CPU (Yield()) s'il
       n'y a pas encore de nouvelle frame à dessiner.
       Valeur initiale 0 : le rendu attend obligatoirement le premier tick logique. */
    Semaphore frameSem { 0 };

    void addSuns(int amount);
    void updateLogic();   /* Pure logique (pas de dessin) */
    void renderFrame();   /* Pure présentation (backbuffer + blit) */
    void drawSunHud();
    void drawLivesHud();
    void drawQueueHud(const PlantQueue& q, int px, int py, unsigned char color);
    void showGameOver();
    void resetGame();
    void handleInput();
    void drawCursor(int col, int row, unsigned char color);
    bool placePlant(int col, int row, PlantType type);
};

#endif
