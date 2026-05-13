#include <Applications/PlantsVsZombies/LivesSystem.h>
#include <vga/vga.h>

static const unsigned char HUD_BG     =  1;
static const unsigned char HUD_BORDER =  3;
static const unsigned char LIFE_ON    = 50;  // rouge foncé

LivesSystem::LivesSystem() : lives(MAX_LIVES) {}

void LivesSystem::loseLife()          { if (lives > 0) lives--; }
bool LivesSystem::isGameOver() const  { return lives <= 0; }

void LivesSystem::draw() const {
    const int px = 220, py = 1, pw = 28, ph = 20;
    for (int row = 0; row < ph; row++)
        for (int col = 0; col < pw; col++)
            video[(py + row) * 320 + (px + col)] = HUD_BG;
    for (int col = 0; col < pw; col++) {
        video[py * 320 + (px + col)]            = HUD_BORDER;
        video[(py + ph - 1) * 320 + (px + col)] = HUD_BORDER;
    }
    for (int k = 0; k < MAX_LIVES; k++) {
        unsigned char c = (k < lives) ? LIFE_ON : HUD_BG;
        plot_square(px + 2 + k * 9, py + 5, 7, c);
    }
}
