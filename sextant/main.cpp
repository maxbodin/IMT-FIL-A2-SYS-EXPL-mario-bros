#include <hal/multiboot.h>
#include <drivers/Clavier.h>
#include <drivers/timer.h>
#include <sextant/interruptions/idt.h>
#include <sextant/interruptions/irq.h>
#include <sextant/interruptions/handler/handler_tic.h>
#include <sextant/interruptions/handler/handler_clavier.h>
#include <sextant/memoire/memoire.h>
#include <vga/vga.h>
#include <Applications/PlantsVsZombies/sprites/peashooter.h>

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

	set_vga_mode13();
	set_palette_vga(peashooter_palette);
	clear_vga_screen(0);

	draw_sprite(peashooter_sprite_data, PEASHOOTER_WIDTH, PEASHOOTER_HEIGHT, 146, 84);

	while (true);
}
