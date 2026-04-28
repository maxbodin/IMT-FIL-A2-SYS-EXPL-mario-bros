#include <Applications/PlantsVsZombies/PlantsVsZombies.h>
#include <Applications/PlantsVsZombies/sprites/shared_palette.h>
#include <sextant/memoire/memoire.h>
#include <vga/vga.h>

PlantsVsZombies::PlantsVsZombies() : plantCount(0) {
    for (int i = 0; i < MAX_PLANTS; i++)
        plants[i] = 0;
}

void PlantsVsZombies::init() {
    set_vga_mode13();
    set_palette_vga(shared_palette);
    clear_vga_screen(0);
}

void PlantsVsZombies::start() {
    grid.render();

    int px, py;
    grid.tileToPixel(0, 0, px, py);
    plants[0] = new Peashooter(px, py);
    plantCount = 1;
    plants[0]->render();

    while (true);
}
