/*
 * memoire.cpp
 *
 *  Created on: 27 ao�t 2008
 *      Author: jmenaud
 */
#include "memoire.h"

void *debut;
int taille;
Ecran *ec;

void mem_setup(void * begin, int size,Ecran *ecran) {
	debut=begin;
	taille=size;
	ec=ecran;
}


void *getmem(long unsigned int nbytes)
{
	void *adresse;
	int i = (int) debut;

	ec->sautDeLigne();
	ec->afficherMot("Adresse de debut : ");
	ec->afficherBase((unsigned int)i,16,BLANC);
	ec->sautDeLigne();

	ec->afficherMot("Taille passee en param de getmem : ");
	ec->afficherBase((unsigned int)nbytes,16,BLANC);
	ec->sautDeLigne();

	i += nbytes;

	adresse = debut;
	
	ec->afficherMot("Adresse de fin : ");
	ec->afficherBase((unsigned int)i,16,BLANC);
	ec->sautDeLigne();

	debut = (void *) i;

	return adresse;
}

void* operator new(long unsigned int taille_objet) {
	const long unsigned int taille_entete = sizeof(long unsigned int);
	void *temp = getmem(taille_objet + taille_entete);

	*((long unsigned int *)temp) = taille_objet;
	void *objet = (void *)((unsigned char *)temp + taille_entete);
	
	ec->afficherMot("Entree dans constructeur, adresse :  ",BLANC);
	ec->afficherBase((long unsigned int)objet,16,BLANC);
	ec->sautDeLigne();
	ec->afficherMot("Entree dans constructeur, taille :  ",BLANC);
	ec->afficherBase((long unsigned int)taille_objet,16,BLANC);
	ec->sautDeLigne();

	return objet;
}

void operator delete(void *object)
{

}

void freemem(int address, int size) {
	if (address == 0 || size <= 0) {
        return;
    }

    unsigned char *zone = (unsigned char *)address;
    for (int i = 0; i < size; i++) {
        zone[i] = 0;
    }
}