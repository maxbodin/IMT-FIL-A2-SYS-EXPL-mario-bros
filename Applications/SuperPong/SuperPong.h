#ifndef CASSEBRIQUE_H
#define CASSEBRIQUE_H

#include <drivers/Clavier.h>

#include "config.h"
#include "Grille.h"
#include "Balle.h"

/**
 * @file SuperPong.h
 * @class SuperPong
 * @brief Le jeu Pong avec deux raquettes, plusieurs balles et de jolies couleurs.
 */



class SuperPong {
	Grille laGrille;
	Clavier *clavier;
	Ecran *ecran;
public:
	void start();
	void init(Ecran*,Clavier*);
};
#endif
