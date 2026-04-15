#include "Raquette.h"

Raquette::Raquette(int taille,int colonne){
	bas=13 + taille/2;
	haut = 13-taille/2;
	y=colonne;
}

Raquette::Raquette(){
}

void Raquette::init(int taille,int colonne){
	bas=12 + taille/2;
	haut = 12-taille/2;
	y=colonne;
}

int Raquette::getY() {
	return y;
}
int Raquette::getBas(){
	return bas;
}
int Raquette::getHaut(){
	return haut;
}

void Raquette::monter(){
	haut--;
	bas--;
}

void Raquette::descendre(){
	haut++;
	bas++;
}

void Raquette::setBas(int i){
	bas = i;
}

void Raquette::setHaut(int i){
	haut = i;
}

void Raquette::setY(int i){
	y = i;
}
