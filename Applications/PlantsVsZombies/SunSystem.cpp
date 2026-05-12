#include <Applications/PlantsVsZombies/SunSystem.h>
#include <vga/vga.h>

static const unsigned char HUD_BG     =   1;
static const unsigned char HUD_BORDER =   3;
static const unsigned char HUD_TEXT   =  15;
static const unsigned char HUD_YELLOW =  25;
static const unsigned char HUD_GREEN  = 205;

static void draw_plus(int x, int y, unsigned char color, int scale) {
    plot_square(x + scale,      y,           scale, color);
    plot_square(x,              y + scale,   scale, color);
    plot_square(x + scale,      y + scale,   scale, color);
    plot_square(x + scale * 2,  y + scale,   scale, color);
    plot_square(x + scale,      y + scale*2, scale, color);
}

SunSystem::SunSystem() : suns(INITIAL), lastTick(0), displayEnd(0) {}

void SunSystem::init(int compt) { lastTick = compt; }

bool SunSystem::canAfford(int cost) const { return suns >= cost; }
void SunSystem::spend(int cost) { if (suns >= cost) suns -= cost; }

void SunSystem::update(int compt) {
    if (compt - lastTick >= TICK_INTERVAL) {
        suns      += TICK_AMOUNT;
        lastTick   = compt;
        displayEnd = compt + DISPLAY_DURATION;
    }
}

void SunSystem::draw(int compt) const {
    const int px = 120, py = 1, pw = 80, ph = 20;
    for (int row = 0; row < ph; row++)
        for (int col = 0; col < pw; col++)
            video[(py + row) * 320 + (px + col)] = HUD_BG;
    for (int col = 0; col < pw; col++) {
        video[py * 320 + (px + col)]             = HUD_BORDER;
        video[(py + ph - 1) * 320 + (px + col)]  = HUD_BORDER;
    }
    plot_square(px + 4, py + 3, 14, HUD_YELLOW);
    draw_number(suns, px + 22, py + 6, HUD_TEXT, 2);
    if (compt < displayEnd) {
        draw_plus(px + 52, py + 6, HUD_GREEN, 2);
        draw_number(TICK_AMOUNT, px + 60, py + 6, HUD_GREEN, 2);
    }
}
