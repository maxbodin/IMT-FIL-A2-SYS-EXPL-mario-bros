#include "Balle.h"

/**
 * @file Balle.cpp
 * Implémentation des méthodes de la classes Balle et initialisation de id à 0.
 */

Balle::Balle(Grille* laGrille) {

	Random random;

	grille = laGrille;
	vitesse = VITESSE;
	// Choisir une positon aleatoire hors de la zone gris'e :
	y = 1 + random.nextInt(LARGEUR);
	x = 1 + random.nextInt(HAUTEUR);

	// Occuper la case :
	grille->getCase(x,y)->occupe();
	// Choisir un deplacement aleatoire h_d, h_g, b_d, b_g:
	dx = 1 ;  if(random.nextBool()) dx = -dx ;
	dy = 1 ;  if(random.nextBool()) dy = -dy ;
};


void Balle::bouger() {
	//Avancer de (dx, dy) :
	int nx = x + dx ;
	int ny = y + dy ;
	//Deplacement : informer les cases de la presence ou non de la balle :
	grille->getCase(nx,ny)->occupe();
	grille->getCase(x,y)->libere();
	// Nouvelle position :
	x = nx ; y = ny ;
}


bool Balle::avance(){//Retourne false si la balle est perdu
	for(int i=0;i<vitesse;i++);
	//Collision avec les bords :
	switch(grille->getCase(x+dx,y)->Etat()){
	case BORD :
		dx = -dx;
		break;
	case  TROU :
		dx = 0;
		dy = 0;
		grille->getCase(x,y)->libere() ;
		grille->getCase(x,y)->paint();
		return false;
	}
	switch(grille->getCase(x,y+dy)->Etat()){
	case BORD :
		dy = -dy;
		break;
	case TROU :
		dx = 0;
		dy = 0;
		grille->getCase(x,y)->libere() ;
		grille->getCase(x,y)->paint();
		return false;
	}
	switch(grille->getCase(x+dx,y+dy)->EtatOld()) {//Evaluation des actions d'apres la case future
	case BORD :
		dy = -dy;
		dx = -dx;
		break;
	}
	grille->getCase(x,y)->libere() ;
	grille->getCase(x,y)->paint();
	x=x+dx;y=y+dy;
	grille->getCase(x,y)->occupe();
	grille->getCase(x,y)->paint();
	return true;


}
