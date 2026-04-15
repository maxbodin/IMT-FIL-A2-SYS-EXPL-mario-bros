#ifndef CASE_H
#define CASE_H

/**
 * @file Case.h
 * @class Case
 * @brief Une case de la grille de jeu.
 */


#include <drivers/Ecran.h>
#include "config.h"

class Case {
	/**
	 * écran pour l'affichage de la case
	 */
	Ecran* ecran;
	/**
	 * coordonnées de la case dans la grille.
	 */
    int x, y ;
    /**
     * Etat de la case. Permet de savoir si la case est un trou, un mur, une raquette ou une balle.
     */
    int etat;
    int etatold;
    /**
     * Couleur du premier plan de la case.
     */
    Couleur couleurPr();
    /**
     * Couleur de l'arrière plan de la case.
     */
    Couleur couleurAr();
    /**
     * Caractère de la case.
     */
    char Caractere();

public:
	//Modifieurs
	void setEcran(Ecran* e);
	void setCaseBord();
    void setCaseTrou();
    void setCaseVide();
    void setCaseCompt(int);
    void setCaseZone(int=1);
    //Accesseurs
    Ecran* getEcran();
    int getEtat();
    /**
     * Permet de connaître le nombre de balles perdues pendant le jeu.
     */
    int nbBallePerdue(int);
    /**
     * définit la case x,y comme une case vide.
     */
    void init(int x, int y);

    /**
     * Affiche la case.
     */
    void paint();


    void occupe();
    void libere();
    int Etat();
    int EtatOld();

};
#endif
