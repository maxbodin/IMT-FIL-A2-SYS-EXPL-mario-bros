#include "SuperPong.h"

void SuperPong::init(Ecran* e,Clavier* c) {
	ecran=e;
	clavier=c;
	e->effacerEcran(NOIR);
	laGrille.init(e);
	laGrille.affiche();
}


void SuperPong::start() {
	char buffer[256];
	int  i;
	char *src;
	Balle balle(&laGrille);

	//Modification de la routine d'interruption du keyboard
   //	irq_set_routine(IRQ_KEYBOARD, keyboard_handler_jeu);
	//Initialisation de la grille

//	while (true) balle.avance();

		if(clavier->testChar()) {

			src=clavier->getString();

			for(i=0;i<256-1 && src[i]!='\0';i++)
				buffer[i]=src[i];
			buffer[i]='\0';

			for(i=0;buffer[i]!='\0';i++)
				switch(buffer[i]){
				case 's' :
					laGrille.monterRaquette(GAUCHE);
					break;
				case 'x' :
					laGrille.descendreRaquette(GAUCHE);
					break;
				case 'j' :
					laGrille.monterRaquette(DROITE);
					break;
				case 'n' :
					laGrille.descendreRaquette(DROITE);
					break;
				}
		}
	}
