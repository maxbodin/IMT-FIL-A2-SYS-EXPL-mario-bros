#ifndef PLANTSVSZOMBIES_H
#define PLANTSVSZOMBIES_H

#include <Applications/PlantsVsZombies/Grid.h>
#include <Applications/PlantsVsZombies/Peashooter.h>
#include <Applications/PlantsVsZombies/Bullet.h>
#include <Applications/PlantsVsZombies/Zombie.h>
#include <drivers/Clavier.h>

#define MAX_PLANTS  45
#define MAX_BULLETS 90
#define MAX_ZOMBIES 5

#define COLLISION_DISTANCE 5
#define ZOMBIE_DAMAGE      25

#define SUN_INITIAL           150
#define SUN_TICK_INTERVAL   10000  // ticks entre chaque gain automatique (10 s à 1000 Hz)
#define SUN_TICK_AMOUNT        25  // soleils gagnés à chaque intervalle
#define SUN_DISPLAY_DURATION 2000  // durée d'affichage du "+Y" en ticks (2 s)

class PlantsVsZombies {
public:
    PlantsVsZombies();

    void init(Ecran* e,Clavier* c);
    void start();

    bool canAfford(int cost) const;
    void spendSuns(int cost);
    void addSuns(int amount);

private:
    Grid        grid;
    Peashooter* plants[MAX_PLANTS];
    int         plantCount;
    Bullet*     bullets[MAX_BULLETS];
    int         bulletCount;
    Zombie*     zombies[MAX_ZOMBIES];
    int         zombieCount;

    Ecran*      ecran;
    Clavier*    clavier;
    unsigned char*  backbuffer;

    int         suns              { SUN_INITIAL };
    int         lastSunTick       { 0 };
    int         sunGainDisplayEnd { 0 };
    int         lastFps           { 0 };
    int         lastSeconds       { 0 };

    void update_screen();
    void drawSunHud();
};

#endif
