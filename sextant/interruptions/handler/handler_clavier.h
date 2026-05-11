/*
 * handler_clavier.h
 *
 *  Created on: 8 ao�t 2008
 *      Author: jmenaud
 */

#ifndef HANDLER_CLAVIER_H_
#define HANDLER_CLAVIER_H_

#define CAPS 4
#define NUM 2
#define SCROLL 1
#define KBD_NOMODIFIER   0
#define KBD_SHIFT        1
#define KBD_LEFTSHIFT    2
#define KBD_RIGHTSHIFT   4
#define KBD_CTRL         8
#define KBD_LEFTCTRL    16
#define KBD_RIGHTCTRL   32
#define KBD_ALT         64
#define KBD_ALTGR      128
#define USERKEY 300
#define BS USERKEY+1
#define EN USERKEY+2
#define F1 USERKEY+3
#define F2 USERKEY+4
#define F3 USERKEY+5
#define F4 USERKEY+6
#define F5 USERKEY+7
#define F6 USERKEY+8
#define F7 USERKEY+9
#define F8 USERKEY+10
#define F9 USERKEY+11
#define F10 USERKEY+12
#define TAB USERKEY+13
#define K16 'a'
#define K17 'z'
#define K18 'e'
#define K19 'r'
#define K20 't'
#define K21 'y'
#define K22 'u'
#define K23 'i'
#define K24 'o'
#define K25 'p'
#define K30 'q'
#define K31 's'
#define K32 'd'
#define K33 'f'
#define K34 'g'
#define K35 'h'
#define K36 'j'
#define K37 'k'
#define K38 'l'
#define K39 'm'
#define K44 'w'
#define K45 'x'
#define K46 'c'
#define K47 'v'
#define K48 'b'
#define K49 'n'
#define K50 ','

/* [EXPLICATION] Scan codes physiques des touches de jeu (indépendants du layout clavier).
   Un scan code identifie la POSITION physique de la touche sur le clavier,
   pas le caractère imprimé — ce qui permet un fonctionnement correct
   quelle que soit la disposition (AZERTY, QWERTY…).                        */

/* Joueur 1 : ZQSD + Espace */
#define SC_P1_UP    17   /* touche physique W (QWERTY) = Z (AZERTY) */
#define SC_P1_LEFT  30   /* touche physique A (QWERTY) = Q (AZERTY) */
#define SC_P1_DOWN  31   /* touche physique S */
#define SC_P1_RIGHT 32   /* touche physique D */
#define SC_P1_PLACE 57   /* Espace */

/* Joueur 2 : IJKL + O */
#define SC_P2_UP    23   /* I */
#define SC_P2_LEFT  36   /* J */
#define SC_P2_DOWN  37   /* K */
#define SC_P2_RIGHT 38   /* L */
#define SC_P2_PLACE 24   /* O */

/* Curseurs rosters (sélection dans la file de plantes) */
#define SC_P1_ROSTER_LEFT  44   /* W (QWERTY) */
#define SC_P1_ROSTER_RIGHT 45   /* X (QWERTY) */
#define SC_P2_ROSTER_LEFT  21   /* Y */
#define SC_P2_ROSTER_RIGHT 22   /* U */

/* [EXPLICATION] File d'événements clavier — architecture producteur/consommateur.
   Le handler IRQ (producteur) push() les événements ; la boucle de jeu (consommateur)
   les pop() via tryP() sous section critique (disable_IRQs).
   Voir sextant/sync/KeyboardQueue.h pour le détail du protocole.                */
#include <sextant/sync/KeyboardQueue.h>

void handler_clavier(int irq);


#endif /* HANDLER_CLAVIER_H_ */
