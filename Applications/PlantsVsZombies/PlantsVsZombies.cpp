#include <Applications/PlantsVsZombies/PlantsVsZombies.h>
#include <Applications/PlantsVsZombies/sprites/peashooter.h>
#include <sextant/memoire/memoire.h>

PlantsVsZombies::PlantsVsZombies() : _plantCount(0) {
    for (int i = 0; i < MAX_PLANTS; i++)
        _plants[i] = 0;
}

void PlantsVsZombies::init() {
    set_vga_mode13();
    set_palette_vga(peashooter_palette);
    clear_vga_screen(0);
}

void PlantsVsZombies::update() {
    updateEntities();
    cleanDeadEntities();
}

void PlantsVsZombies::render() {
    clear_vga_screen(0);
    _grid.render();
    renderEntities();
}

void PlantsVsZombies::placePlant(int col, int row) {
    Tile* tile = _grid.getTile(col, row);
    if (!tile || !tile->isEmpty()) return;
    if (_plantCount >= MAX_PLANTS) return;

    int px, py;
    _grid.tileToPixel(col, row, px, py);

    Peashooter* p = new Peashooter(px, py);
    _plants[_plantCount++] = p;
    tile->setHasPlant(true);
}

void PlantsVsZombies::updateEntities() {
    for (int i = 0; i < _plantCount; i++) {
        if (_plants[i])
            _plants[i]->update();
    }
}

void PlantsVsZombies::renderEntities() {
    for (int i = 0; i < _plantCount; i++) {
        if (_plants[i])
            _plants[i]->render();
    }
}

void PlantsVsZombies::cleanDeadEntities() {
    for (int i = 0; i < _plantCount; i++) {
        if (_plants[i] && _plants[i]->isDead()) {
            delete _plants[i];
            _plants[i] = 0;
        }
    }
}
