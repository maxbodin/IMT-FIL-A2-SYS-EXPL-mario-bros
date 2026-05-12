#include <Applications/PlantsVsZombies/DmgIndicator.h>
#include <vga/vga.h>

extern volatile int compt;

/* Palette index 25 = yellow (shared_palette). */
static const unsigned char DMG_COLOR = 25;

DmgIndicator::DmgIndicator()
    : x(0), y(0), value(0), startTick(0), duration(0), active(false) {}

void DmgIndicator::init(int x, int y, int value, int duration) {
    this->x         = x;
    this->y         = y;
    this->value     = value;
    this->startTick = compt;
    this->duration  = duration;
    this->active    = true;
}

void DmgIndicator::update() {
    if (!active) return;
    if (compt - startTick >= duration)
        active = false;
}

void DmgIndicator::render() {
    if (!active) return;
    int elapsed = compt - startTick;
    int floatY  = y - (elapsed / 50);
    draw_number(value, x, floatY, DMG_COLOR, 1);
}

bool DmgIndicator::isActive() const { return active; }
void DmgIndicator::deactivate()     { active = false; }
