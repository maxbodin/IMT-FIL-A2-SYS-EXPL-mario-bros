#include <hal/multiboot.h>
#include <drivers/Clavier.h>
#include <drivers/timer.h>
#include <sextant/interruptions/idt.h>
#include <sextant/interruptions/irq.h>
#include <sextant/interruptions/handler/handler_tic.h>
#include <sextant/interruptions/handler/handler_clavier.h>
#include <sextant/memoire/memoire.h>
#include <sextant/ordonnancements/preemptif/thread.h>
#include <Applications/PlantsVsZombies/PlantsVsZombies.h>

extern char __e_kernel;

extern "C" void Sextant_main(unsigned long magic, unsigned long addr){
	Clavier clavier;
	Timer timer;

	multiboot_info_t* mbi = (multiboot_info_t*)addr;

	idt_setup();
	irq_setup();
	timer.i8254_set_frequency(1000);
	irq_set_routine(IRQ_TIMER, ticTac);
	irq_set_routine(IRQ_KEYBOARD, handler_clavier);
	asm volatile("sti\n");

	mem_setup(&__e_kernel, (mbi->mem_upper << 10) + (1 << 20), 0);

	/* [ORDONNANCEMENT] Initialisation du sous-système de threads.
	   - sched_subsystem_setup() initialise les files de l'ordonnanceur Round-Robin.
	   - thread_subsystem_setup() enregistre le thread initial (celui qui exécute
	     ce code) comme premier thread du système, en lui associant sa pile.
	   L'adresse de pile est lue depuis le registre ESP au moment de l'appel.
	   La taille (16 Ko) est une estimation conservatrice pour un thread kernel. */
	sched_subsystem_setup();
	{
		vaddr_t esp;
		asm volatile("movl %%esp, %0" : "=r"(esp));
		thread_subsystem_setup(esp - 0x4000, 0x4000);
	}

	PlantsVsZombies pvzgame;
	pvzgame.init(&Ecran(), &clavier);
	pvzgame.start();  /* Crée les threads du jeu et cède le CPU. Ne retourne jamais. */

}
