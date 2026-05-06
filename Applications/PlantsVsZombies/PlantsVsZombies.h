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

class PlantsVsZombies {
public:
    PlantsVsZombies();

    void init(Ecran* e,Clavier* c);
    void start();

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

    int         lastFps     { 0 };
    int         lastSeconds { 0 };  

    void update_screen();
};

#endif
