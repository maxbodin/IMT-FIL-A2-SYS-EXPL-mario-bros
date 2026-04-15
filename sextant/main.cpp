#include <hal/multiboot.h>
#include <drivers/Ecran.h>
#include <Applications/SuperPong/SuperPong.h>
#include <drivers/PortSerie.h>

#include <sextant/interruptions/idt.h>
#include <sextant/interruptions/irq.h>
#include <sextant/interruptions/handler/handler_tic.h>
#include <sextant/interruptions/handler/handler_clavier.h>
#include <drivers/timer.h>
#include <drivers/Clavier.h>
#include <Applications/Horloge/Horloge.h>
#include <sextant/memoire/memoire.h>

extern char __e_kernel,__b_kernel, __b_data, __e_data,  __b_stack, __e_load ;
int i;

void func2(int c,int d, Ecran *ec) {
	int *p;
	/* QUESTION 1 */

	ec->sautDeLigne();
	ec->afficherMot("Adresse de c : ",BLANC);
	ec->afficherBase((unsigned int)&c,16,BLANC);
	ec->sautDeLigne();
	ec->afficherMot("Adresse de d : ",BLANC);
	ec->afficherBase((unsigned int)&d,16,BLANC);
	ec->sautDeLigne();

	/* FIN QUESTION 1 */

}

void func(int a,int b, Ecran *ec) {
	int *p;
	func2(a,b,ec);

	/* QUESTION 1 */

	ec->sautDeLigne();
	ec->afficherMot("Adresse de a : ",BLANC);
	ec->afficherBase((unsigned int)&a,16,BLANC);
	ec->sautDeLigne();
	ec->afficherMot("Adresse de b : ",BLANC);
	ec->afficherBase((unsigned int)&b,16,BLANC);
	ec->sautDeLigne();

	/* FIN QUESTION 1 */

	/* QUESTION 2 */
	 p = &a ; /* p pointe sur l'adresse de a */
	 /* soustraire 4 octets de p (ou &b) si bien */
	 /* qu'il pointe sur l'adresse de retour */
	 /* ne pas oublier que si p est un entier (int) alors p = p+4 <==> p = p +4*(sizeof (int)) */
	 /* ici les entiers sont cod�s sur 4 octets */

	ec->afficherMot("Adresse ou est empile le retour de la fonction : ",BLANC);
	ec->afficherBase((unsigned int)(p - 1),16,BLANC);
	ec->sautDeLigne();

	ec->afficherMot("Adresse de retour de la fonction : ",BLANC);
	ec->afficherBase((unsigned int)*(p - 1),16,BLANC);
	ec->sautDeLigne();

	/* FIN QUESTION 2 */

	/* QUESTION 3 et 4 : produire un fichier du binaire desassemble */

	// echo '_ZN5Ecran11afficherMotEPKc7Couleur' | c++filt => Ecran::afficherMot(char const*, Couleur)

	/* QUESTION 5 */

	/* ajouter/soustraire xx octets a l'adresse de retour */
	 /* * to skip the i = 1 */
	 *(p - 1) += 13;

	 ec->afficherMot("Nouvelle adresse de retour de la fonction : ",BLANC);
	 ec->afficherBase((unsigned int)*(p - 1),16,BLANC);
	 ec->sautDeLigne();

	 /* FIN QUESTION 5 */

}

extern "C" void Sextant_main(unsigned long magic, unsigned long addr){
	char beuf[250];
	Ecran ecran;
	SuperPong *test;
	PortSerie ps;
	Timer timer;
//	Grille grille(&ecran);
	Horloge h(&ecran,&timer);
	Clavier clavier;
	SuperPong jeux;


	ecran.effacerEcran(NOIR);

	idt_setup();
	irq_setup();
	//Initialisation de la fréquence de l'horloge
	timer.i8254_set_frequency(1000);

	irq_set_routine(IRQ_TIMER, ticTac);
	irq_set_routine(IRQ_KEYBOARD, handler_clavier);
	asm volatile("sti\n");//Autorise les interruptions

	//i8259_disable_irq_line(IRQ_KEYBOARD);

	jeux.init(&ecran,&clavier);

	// while(true) {
	// 	h.afficheHeure();
	// 	if (clavier.testChar()) jeux.start();
	// }

	/* PARTIE I  */

	i=0;
	func(1,2,&ecran);
	i=1;
	ecran.afficherMot("Valeur de i : ",BLANC);
	ecran.afficherBase((unsigned int)i,10,BLANC);
	ecran.sautDeLigne();


	/* PARTIE II  */

	multiboot_info_t* mbi;
	mbi = (multiboot_info_t*)addr;

	mem_setup(& __e_kernel,(mbi->mem_upper<<10) + (1<<20),&ecran);

	test = new SuperPong;
	test = new SuperPong;

	unsigned char *zoneTest = (unsigned char *)test;
	zoneTest[0] = 0xAA;
	zoneTest[1] = 0x55;

	ecran.afficherMot("Avant freemem : ", BLANC);
	ecran.afficherBase((unsigned int)zoneTest[0], 16, BLANC);
	ecran.afficherBase((unsigned int)zoneTest[1], 16, BLANC);
	ecran.sautDeLigne();

	freemem((int)test, sizeof(SuperPong));

	ecran.afficherMot("Apres freemem : ", BLANC);
	ecran.afficherBase((unsigned int)zoneTest[0], 16, BLANC);
	ecran.afficherBase((unsigned int)zoneTest[1], 16, BLANC);
	ecran.sautDeLigne();

	Ecran *ec ;
	ec = new Ecran ;
	ec->afficherMot("Test1") ;
	delete ec ;
	ec->afficherMot("Test2") ;


	while (true); /* Pour stopper l'execution a cet endroit */
}
