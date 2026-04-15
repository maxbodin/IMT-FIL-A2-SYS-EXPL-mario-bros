#include "Case.h"

/**
 * @file Case.cpp
 * Implémentation des méthodes de la classes Case.
 */

void Case::init(int xx, int yy) {
	x = xx ; y = yy ;
	setCaseVide();
};

int Case::getEtat() {
	return etat;
};

void Case::setEcran(Ecran* e) {
	ecran = e;
}

Ecran* Case::getEcran() {
	return ecran;
}

void Case::setCaseVide(){
	etat=VIDE;
};

void Case::setCaseTrou(){
	etat=TROU;
};

void Case::setCaseBord(){
	etat=BORD;
};

void Case::paint(){
	ecran->afficherCaractere(x, y, couleurPr(),couleurAr(),Caractere());
};

Couleur Case::couleurPr() {
	switch(etat){
	case VIDE :
		return NOIR;
	case BORD :
		return JAUNE;
	case TROU :
		return NOIR;
	default :
		return BLEU_LEGER;
	}
}

Couleur Case::couleurAr() {
		return GRIS_SOMBRE;
}

char Case::Caractere(){
	switch(etat){
	case VIDE :
		return ' ';
	case BORD :
	case TROU :
		return 219;
	case BALLE :
		return '0';
	default :
		return 'X';
	}
}

/* Ajout TP2
 */

void Case::occupe() {
	etatold=etat;
	etat = BALLE;
	paint();
};

void Case::libere() {
	etat=etatold;
	paint();
};

int Case::EtatOld() {
	return etatold;
};

int Case::Etat() {
	return etat;
};

