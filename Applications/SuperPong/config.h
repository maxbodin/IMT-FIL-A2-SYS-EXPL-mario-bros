#ifndef CONFIG_H
#define CONFIG_H

/***
 * @file config.h
 * @brief Fichier de configuration du jeu. 
 * N.B. : Les zones doivent être entourées par un espace vide. 
 * Les zones doivent être inclues les unes dans les autres
 ***/

/*** Type des cases ***/
#define VIDE 		0
#define BORD 		10
#define TROU 		20
#define COMPTEUR 	30
#define BALLE 		40
#define ZONE 		50
/*** Taille du jeu ***/
#define LARGEUR 	80
#define HAUTEUR 	25
/*** Nombre de zones maximum ***/
#define NB_ZONE 	3
/*** Caract�ristiques de la zone 1 (zone la plus grande) ***/
#define LARGZONE1 	45
#define HAUTZONE1 	20
#define NB_VERROU1 	2
/*** Caract�ristiques de la zone 2 (zone plus petite que la zone 1) ***/
#define LARGZONE2 	30
#define HAUTZONE2 	15
#define NB_VERROU2 	2
/*** Caract�ristiques de la zone 3 (zone plus petite que la zone 2) ***/
#define LARGZONE3 	20
#define HAUTZONE3 	8
#define NB_VERROU3 	1
/*** Nombre de balles maximum ***/
#define BALLE_MAX 	30
/*** Vitesse des balles ***/
#define VITESSE 	400000
/*** Choix de la raquette ***/
#define GAUCHE 		1
#define DROITE 		2

#endif
