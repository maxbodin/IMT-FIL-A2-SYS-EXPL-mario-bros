#include "Grille.h"


Grille::Grille(){
}


Grille::Grille(Ecran* e){
	//Initialisation de la grille
	for(int x=0 ; x<HAUTEUR ; x++) {
		for(int y=0 ; y<LARGEUR ; y++) {
			tab[x][y].init(x,y);
			tab[x][y].setEcran(e);
			if(x==0 || x==(HAUTEUR-1))//Installation des bords
				tab[x][y].setCaseBord();
			if(y==0)
				if(x > HAUTEUR/3 && x< HAUTEUR*2/3)
					tab[x][y].setCaseTrou();//Installation du trou à gauche
				else
					tab[x][y].setCaseBord();
			if(y==(LARGEUR-1))
				if(x > HAUTEUR/3 && x< HAUTEUR*2/3)
					tab[x][y].setCaseTrou();//Installation du trou à droite
				else
					tab[x][y].setCaseBord();

		}
	}
	initRaquette(3,4);
};

void Grille::init(Ecran* e){
	//Initialisation de la grille
	for(int x=0 ; x<HAUTEUR ; x++) {
		for(int y=0 ; y<LARGEUR ; y++) {
			tab[x][y].init(x,y);
			tab[x][y].setEcran(e);
			if(x==0 || x==(HAUTEUR-1))//Installation des bords
				tab[x][y].setCaseBord();
			if(y==0)
				if(x > HAUTEUR/3 && x< HAUTEUR*2/3)
					tab[x][y].setCaseTrou();//Installation du trou à gauche
				else
					tab[x][y].setCaseBord();
			if(y==(LARGEUR-1))
				if(x > HAUTEUR/3 && x< HAUTEUR*2/3)
					tab[x][y].setCaseTrou();//Installation du trou à droite
				else
					tab[x][y].setCaseBord();

		}
	}
	initRaquette(3,4);
};

void Grille::monterRaquette(int gd){
	if(gd == GAUCHE) {
		if(raqg.getHaut()>1) {
			tab[raqg.getBas()][raqg.getY()].setCaseVide();
			tab[raqg.getBas()][raqg.getY()].paint();
			raqg.monter();
			tab[raqg.getHaut()][raqg.getY()].setCaseBord();
			tab[raqg.getHaut()][raqg.getY()].paint();
		}
	}
	else {
		if(raqd.getHaut()>1) {
			tab[raqd.getBas()][raqd.getY()].setCaseVide();
			tab[raqd.getBas()][raqd.getY()].paint();
			raqd.monter();
			tab[raqd.getHaut()][raqd.getY()].setCaseBord();
			tab[raqd.getHaut()][raqd.getY()].paint();
		}
	}
}

void Grille::descendreRaquette(int gd){
	if(gd == GAUCHE) {//Déplacement de la raquette de gauche
		if(raqg.getBas()<HAUTEUR-2) {
			tab[raqg.getHaut()][raqg.getY()].setCaseVide();
			tab[raqg.getHaut()][raqg.getY()].paint();
			raqg.descendre();
			tab[raqg.getBas()][raqg.getY()].setCaseBord();
			tab[raqg.getBas()][raqg.getY()].paint();
		}
	}
	else {//Déplacement de la raquette de droite
		if(raqd.getBas()<HAUTEUR-2) {
			tab[raqd.getHaut()][raqd.getY()].setCaseVide();
			tab[raqd.getHaut()][raqd.getY()].paint();
			raqd.descendre();
			tab[raqd.getBas()][raqd.getY()].setCaseBord();
			tab[raqd.getBas()][raqd.getY()].paint();
		}
	}
}

void Grille::initRaquette(int y,int lg){ // colonne, taille

	raqg.init(lg,y);
//	raqg.setBas(haut+lg);
//	raqg.setHaut(haut);

	raqd.init(lg,LARGEUR -1 - y);
//	raqd.setBas(haut+lg);
//	raqd.setHaut(haut);

	for(int i=0;i<=lg;i++) {
		tab[raqg.getHaut()+i][raqg.getY()].setCaseBord();
		tab[raqd.getHaut()+i][raqd.getY()].setCaseBord();
	}
}

Case* Grille::getCase(int x,int y) {
	return &tab[x][y];
}

void Grille::affiche(){
	for (int x=0 ; x<HAUTEUR ; x++) {
		for (int y=0 ; y<LARGEUR ; y++) {
			tab[x][y].paint();
		}
	}
};
