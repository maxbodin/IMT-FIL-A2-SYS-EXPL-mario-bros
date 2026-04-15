#ifndef RANDOM_H
#define RANDOM_H

/**
 * @file Random.h
 * @class Random
 * @brief Classe permettant de choisir un entier aléatoirement. 
*/

class Random {
	unsigned long int random(void);

public :
	int nextInt(int);
	bool nextBool(void);
};
#endif


