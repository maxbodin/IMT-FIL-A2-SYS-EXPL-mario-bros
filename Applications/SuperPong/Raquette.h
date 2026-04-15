#ifndef RAQUETTE_H
#define RAQUETTE_H

/**
 * @file Raquette.h
 * @class Raquette
 * @brief Une raquette du jeu pouvant se déplacer verticalement.
*/

class Raquette {
	int y;//Coordonnée
	int bas,haut;//haut et bas de la raquette
public :
	//Accesseurs
	Raquette();
	Raquette(int,int);
	void init(int,int);
	int getY();
	int getBas();
	int getHaut();
	//Modifieurs
	void setBas(int);
	void setHaut(int);
	void setY(int);
	//Méthodes utiles
	void monter();
	void descendre();
};
#endif
