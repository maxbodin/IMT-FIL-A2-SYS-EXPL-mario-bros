#include <hal/multiboot.h>
#include <drivers/Clavier.h>
#include <drivers/timer.h>
#include <sextant/interruptions/idt.h>
#include <sextant/interruptions/irq.h>
#include <sextant/interruptions/handler/handler_tic.h>
#include <sextant/interruptions/handler/handler_clavier.h>
#include <sextant/memoire/memoire.h>
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

	PlantsVsZombies pvzgame;
	pvzgame.init(&Ecran(), &clavier);
	pvzgame.start();

}
