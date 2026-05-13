#include <Applications/PlantsVsZombies/MainMenu.h>
#include <Applications/PlantsVsZombies/sprites/ui/titlescreen_bg_sprite.h>
#include <Applications/PlantsVsZombies/sprites/ui/titlescreen_title_sprite.h>
#include <Applications/PlantsVsZombies/sprites/ui/titlescreen_dirt_sprite.h>
#include <Applications/PlantsVsZombies/sprites/ui/titlescreen_grass_sprite.h>
#include <sextant/sync/KeyboardQueue.h>
#include <sextant/interruptions/handler/handler_clavier.h>
#include <sextant/ordonnancements/preemptif/thread.h>
#include <vga/vga.h>

void MainMenu::render() {
    // Full-screen background
    for (int i = 0; i < 320 * 200; i++)
        video[i] = titlescreen_bg_sprite_data[i];

    // Dirt strip (half size, centered, higher)
    int dirtW = TITLESCREEN_DIRT_WIDTH / 1.5;
    int dirtH = TITLESCREEN_DIRT_HEIGHT / 1.5;
    int dirtX = (320 - dirtW) / 2;
    int dirtY = 200 - dirtH - 8;
    draw_sprite_scaled(titlescreen_dirt_sprite_data,
                TITLESCREEN_DIRT_WIDTH, TITLESCREEN_DIRT_HEIGHT,
                dirtX, dirtY, dirtW, dirtH);

    // Grass strip overlapping the top of the dirt (half size, centered)
    int grassW = TITLESCREEN_GRASS_WIDTH / 1.5;
    int grassH = TITLESCREEN_GRASS_HEIGHT / 1.5;
    int grassX = (320 - grassW) / 2;
    int grassY = dirtY - grassH + 5;
    draw_sprite_scaled(titlescreen_grass_sprite_data,
                TITLESCREEN_GRASS_WIDTH, TITLESCREEN_GRASS_HEIGHT,
                grassX, grassY, grassW, grassH);

    // Title centered near the top
    int titleX = (320 - TITLESCREEN_TITLE_WIDTH) / 2;
    draw_sprite(titlescreen_title_sprite_data,
                TITLESCREEN_TITLE_WIDTH, TITLESCREEN_TITLE_HEIGHT,
                titleX, 8);

    // "press space to start" — 20 chars × 8px (stride at scale2) = 160px
    int textW = 20 * 8;
    int textX = (320 - textW) / 2;
    draw_text("press space to start", textX, 168, 15, 2);
}

void MainMenu::show() {
    render();

    // Drain any buffered key events
    KeyEvent evt;
    while (keyboardQueue.pop(evt)) {}

    // Wait for space press
    while (true) {
        if (keyboardQueue.pop(evt)) {
            if (evt.pressed && evt.scanCode == SC_P1_PLACE)
                return;
        } else {
            thread_yield();
        }
    }
}
