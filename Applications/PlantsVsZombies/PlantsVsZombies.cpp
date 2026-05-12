#include <Applications/PlantsVsZombies/PlantsVsZombies.h>
#include <Applications/PlantsVsZombies/SnowPeashooter.h>
#include <Applications/PlantsVsZombies/Sunflower.h>
#include <Applications/PlantsVsZombies/sprites/shared_palette.h>
#include <Applications/PlantsVsZombies/sprites/peashooter_sprite.h>
#include <Applications/PlantsVsZombies/sprites/snow_peashooter_sprite.h>
#include <Applications/PlantsVsZombies/sprites/sunflower_sprite.h>
#include <Applications/PlantsVsZombies/sprites/sun_small_sprite.h>
#include <Applications/PlantsVsZombies/sprites/pvz_background_sprite.h>
#include <Applications/PlantsVsZombies/sprites/zombie_walk_sprite.h>
#include <sextant/interruptions/handler/handler_clavier.h>
#include <sextant/memoire/memoire.h>
#include <sextant/threads/Threads.h>
#include <sextant/ordonnancements/preemptif/thread.h>
#include <vga/vga.h>

/* =========================================================================
 * Threads de jeu — Round-Robin coopératif
 *
 * CHOIX DE L'ALGORITHME : Round-Robin coopératif (Yield() explicite)
 *
 * L'alternative préemptive (sched_clk hooked sur le timer IRQ) est écartée
 * parce que irq_wrappers.S sauvegarde le contexte puis exécute le handler C
 * puis fait "iret". Si le handler appelle cpu_context_switch(), l'"iret"
 * suivant restaure le mauvais pile ← corruption silencieuse.
 *
 * Avec le mode coopératif :
 *   - Un seul thread tourne à la fois (mono-core).
 *   - Pas de race condition réelle entre threads : la mémoire partagée
 *     (positions des entités, suns, lives…) ne peut pas être modifiée
 *     pendant qu'un autre thread la lit — il faut qu'un thread cède
 *     explicitement le CPU pour que l'autre puisse démarrer.
 *   - Le sémaphore frameSem (initial=0) assure que le thread rendu ne tente
 *     de dessiner qu'une fois que le thread logique a terminé une mise à jour.
 *
 * Flux d'exécution (une itération) :
 *   LogicThread : handleInput() + updateLogic() → frameSem.V() → Yield()
 *   RenderThread: tryP(frameSem) → renderFrame() → Yield()
 *   (initial thread) : Yield() en boucle — thread idle
 * ========================================================================= */

/* PvZLogicThread : met à jour l'état du jeu toutes les 16 ms (≈ 62 fps),
   puis signale le thread rendu via frameSem. */
struct PvZLogicThread : public Threads {
    PlantsVsZombies* game;
    void run() override { game->runLogic(); }
};

/* PvZRenderThread : attend le signal du thread logique, dessine la frame. */
struct PvZRenderThread : public Threads {
    PlantsVsZombies* game;
    void run() override { game->runRender(); }
};

extern volatile int compt;

bool PlantsVsZombies::canAfford(int cost) const { return suns >= cost; }

void PlantsVsZombies::spendSuns(int cost) { if (suns >= cost) suns -= cost; }

void PlantsVsZombies::addSuns(int amount) { suns += amount; }

// Palette indices (shared_palette) : 1=gris foncé, 3=gris clair, 15=blanc, 25=jaune, 205=vert
static const unsigned char SUN_HUD_BG     =   1;
static const unsigned char SUN_HUD_TEXT   =  15;
static const unsigned char SUN_HUD_YELLOW =  25;
static const unsigned char SUN_HUD_GREEN  = 205;
static const unsigned char SUN_HUD_RED    =  32;

// Dessine un "+" 3×3 pixels (à l'échelle scale)
static void draw_plus(int x, int y, unsigned char color, int scale) {
    plot_square(x + scale, y,           scale, color); // haut
    plot_square(x,         y + scale,   scale, color); // gauche
    plot_square(x + scale, y + scale,   scale, color); // centre
    plot_square(x + scale*2, y + scale, scale, color); // droite
    plot_square(x + scale, y + scale*2, scale, color); // bas
}

void PlantsVsZombies::drawSunHud() {
    // Panneau centré : x=120, y=1, 80×20 px
    const int px = 120, py = 1, pw = 80, ph = 20;
    for (int row = 0; row < ph; row++)
        for (int col = 0; col < pw; col++)
            video[(py + row) * 320 + (px + col)] = SUN_HUD_BG;

    bool sunFlash = compt < sunFlashEndTick;
    unsigned char sunColor    = sunFlash ? SUN_HUD_RED : SUN_HUD_TEXT;
    unsigned char borderColor = sunFlash ? SUN_HUD_RED : (unsigned char)3;

    for (int col = 0; col < pw; col++) {
        video[py * 320 + (px + col)]            = borderColor;
        video[(py + ph - 1) * 320 + (px + col)] = borderColor;
    }

    // Icône soleil animée (sun_small, 2 frames)
    {
        static int sunHudFrame = 0;
        static int sunHudAnimTick = 0;
        if (++sunHudAnimTick >= 30) {
            sunHudAnimTick = 0;
            sunHudFrame = (sunHudFrame + 1) % SUN_SMALL_FRAMES;
        }
        draw_sprite_scaled(sun_small_frames[sunHudFrame],
                           SUN_SMALL_WIDTH, SUN_SMALL_HEIGHT,
                           px + 2, py + 2, 16, 16);
    }

    draw_number(suns, px + 22, py + 6, sunColor, 2);

    // Affichage "+Y" pendant SUN_DISPLAY_DURATION ticks après un gain automatique
    if (compt < sunGainDisplayEnd) {
        draw_plus(px + 52, py + 6, SUN_HUD_GREEN, 2);
        draw_number(SUN_TICK_AMOUNT, px + 60, py + 6, SUN_HUD_GREEN, 2);
    }

    // Affichage "+X" après collecte de soleil
    if (compt < sunCollectDisplayEnd) {
        draw_plus(px + 52, py + 6, SUN_HUD_YELLOW, 2);
        draw_number(lastSunCollected, px + 60, py + 6, SUN_HUD_YELLOW, 2);
    }
}

/* Couleurs des curseurs (indices dans shared_palette).
   P1 = blanc (index 15), P2 = vert (index 205, même teinte que la barre de vie). */
#define CURSOR_P1_COLOR 15
#define CURSOR_P2_COLOR 205

PlantsVsZombies::PlantsVsZombies() : plantCount(0), zombieCount(0) {
    for (int i = 0; i < MAX_PLANTS; i++)
        plants[i] = 0;
    for (int i = 0; i < MAX_ZOMBIES; i++)
        zombies[i] = 0;
    for (int i = 0; i < MAX_SUNS; i++)
        suns_on_ground[i] = 0;
}

void PlantsVsZombies::init(Ecran* e, Clavier* c) {
    ecran   = e;
    clavier = c;
    backbuffer = (unsigned char*) getmem(320 * 200);
    set_vga_mode13();
    set_palette_vga(shared_palette);
    clear_vga_screen(0);
}

static bool aabb(int ax, int ay, int aw, int ah,
                 int bx, int by, int bw, int bh) {
    return ax < bx + bw && ax + aw > bx
        && ay < by + bh && ay + ah > by;
}

/* [LOGIQUE] Met à jour l'état du jeu : vagues, plantes, zombies, balles, soleils.
   Appelé par runLogic() toutes les 16 ms. Ne touche pas au framebuffer. */
void PlantsVsZombies::updateLogic() {

    // --- Wave spawning ---
    if (zombieCount < MAX_ZOMBIES) {
        Zombie* z = waveManager.update();
        if (z) zombies[zombieCount++] = z;
    }

    // --- Gain automatique de soleils ---
    if (compt - lastSunTick >= SUN_TICK_INTERVAL) {
        addSuns(SUN_TICK_AMOUNT);
        lastSunTick       = compt;
        sunGainDisplayEnd = compt + SUN_DISPLAY_DURATION;
    }

    // --- Plants: update + spawn bullets/suns + remove dead ---
    for (int i = 0; i < plantCount; i++) {
        plants[i]->update();
        if (plants[i]->isDead()) {
            int tc, tr;
            if (grid.pixelToTile(plants[i]->getX(), plants[i]->getY(), tc, tr)) {
                Tile* t = grid.getTile(tc, tr);
                if (t) t->setState(TileState::Empty);
            }
            delete plants[i];
            plants[i] = plants[--plantCount];
            plants[plantCount] = 0;
            i--;
            continue;
        }

        if (plants[i]->hasSunReady() && sunOnGroundCount < MAX_SUNS) {
            suns_on_ground[sunOnGroundCount++] = new Sun(plants[i]->getX(), plants[i]->getY() + plants[i]->getHeight() - 10);
            plants[i]->resetSunTimer();
        }

        if (plants[i]->canShoot()) {
            int bx = plants[i]->getX();
            int by = plants[i]->getY() + plants[i]->getHeight() / 4;
            Bullet* b = bulletPool.acquire();
            if (b) {
                b->init(bx, by, plants[i]->getBulletType());
                plants[i]->resetCooldown();
            }
        }
    }

    // --- Zombies: collision plante + mouvement + suppression ---
    for (int i = 0; i < zombieCount; i++) {
        bool blocked = false;
        for (int p = 0; p < plantCount; p++) {
            int dx = zombies[i]->getX() - (plants[p]->getX() + plants[p]->getWidth());
            int dy = zombies[i]->getY() - plants[p]->getY();
            if (dy < 0) dy = -dy;
            if (dx >= 0 && dx < COLLISION_DISTANCE && dy < Grid::TILE_SIZE) {
                blocked = true;
                if (zombies[i]->canHit()) {
                    plants[p]->takeDamage(ZOMBIE_DAMAGE);
                    zombies[i]->resetCooldown();
                }
                break;
            }
        }
        blocked ? zombies[i]->block() : zombies[i]->unblock();
        zombies[i]->update();

        if (zombies[i]->getX() <= 0) {
            lives--;
            delete zombies[i];
            zombies[i] = zombies[--zombieCount];
            zombies[zombieCount] = 0;
            i--;
            continue;
        }
        if (zombies[i]->isDead()) {
            delete zombies[i];
            zombies[i] = zombies[--zombieCount];
            zombies[zombieCount] = 0;
            i--;
        }
    }

    // --- Bullets: update + collision vs zombies (object pool, no new/delete) ---
    for (int i = 0; i < bulletPool.CAPACITY; i++) {
        Bullet* b = bulletPool.get(i);
        if (!b->isActive()) continue;

        b->update();
        if (!b->isActive()) continue;

        if (b->getX() > b->getSpawnX() + COLLISION_DISTANCE) {
            for (int z = 0; z < zombieCount; z++) {
                if (aabb(b->getX(),  b->getY(),
                         b->getWidth(), b->getHeight(),
                         zombies[z]->getX(),  zombies[z]->getY(),
                         zombies[z]->getWidth(), zombies[z]->getHeight())) {
                    int dmg = b->getDamage();
                    b->onHit(*zombies[z]);
                    bulletPool.release(b);
                    DmgIndicator* di = dmgPool.acquire();
                    if (di) di->init(zombies[z]->getX(),
                                     zombies[z]->getY() - 4,
                                     dmg, DMG_INDICATOR_DURATION);
                    break;
                }
            }
        }
    }

    // --- Suns on ground: update + cursor collection ---
    for (int i = 0; i < sunOnGroundCount; i++) {
        suns_on_ground[i]->update();
        if (!suns_on_ground[i]->isActive()) {
            delete suns_on_ground[i];
            suns_on_ground[i] = suns_on_ground[--sunOnGroundCount];
            suns_on_ground[sunOnGroundCount] = 0;
            i--;
            continue;
        }

        int sc = suns_on_ground[i]->getTileCol();
        int sr = suns_on_ground[i]->getTileRow();
        if ((cursorCol == sc && cursorRow == sr) ||
            (cursorCol2 == sc && cursorRow2 == sr)) {
            int val = suns_on_ground[i]->getValue();
            addSuns(val);
            lastSunCollected     = val;
            sunCollectDisplayEnd = compt + SUN_COLLECT_DISPLAY;
            delete suns_on_ground[i];
            suns_on_ground[i] = suns_on_ground[--sunOnGroundCount];
            suns_on_ground[sunOnGroundCount] = 0;
            i--;
        }
    }
}

/* [RENDU] Compose le backbuffer et le copie dans le framebuffer VGA.
   Appelé par runRender() après que runLogic() a signalé une nouvelle frame
   via frameSem. Aucune modification de l'état du jeu ici. */
void PlantsVsZombies::renderFrame() {
    unsigned char* real_video = (unsigned char*) video;
    video = backbuffer;

    // Background image
    for (int i = 0; i < 320 * 200; i++)
        backbuffer[i] = pvz_background_sprite_data[i];

    grid.render();

    for (int i = 0; i < plantCount; i++)
        if (plants[i]) plants[i]->render();

    for (int i = 0; i < bulletPool.CAPACITY; i++) {
        Bullet* b = bulletPool.get(i);
        if (b->isActive()) b->render();
    }

    for (int i = 0; i < zombieCount; i++)
        if (zombies[i]) zombies[i]->render();

    for (int i = 0; i < sunOnGroundCount; i++)
        if (suns_on_ground[i]) suns_on_ground[i]->render();

    unsigned char c1 = grid.isTileOccupied(cursorCol, cursorRow)   ? SUN_HUD_RED : CURSOR_P1_COLOR;
    unsigned char c2 = grid.isTileOccupied(cursorCol2, cursorRow2) ? SUN_HUD_RED : CURSOR_P2_COLOR;
    drawCursor(cursorCol,  cursorRow,  c1);
    drawCursor(cursorCol2, cursorRow2, c2);

    // Timer (scale 1) with "s" unit
    {
        int len = 0, tmp = lastSeconds;
        if (tmp == 0) len = 1; else while (tmp > 0) { len++; tmp /= 10; }
        draw_number(lastSeconds, 1, 2, 15, 1);
        draw_text("s", 1 + len * 4, 2, 15, 1);
    }

    // FPS (scale 1) with "fps" unit, right-aligned
    {
        int len = 0, tmp = lastFps;
        if (tmp == 0) len = 1; else while (tmp > 0) { len++; tmp /= 10; }
        int totalW = (len + 3) * 4;
        int startX = 319 - totalW;
        draw_number(lastFps, startX, 2, 15, 1);
        draw_text("fps", startX + len * 4, 2, 15, 1);
    }

    // Wave indicator
    {
        draw_text("w", 1, 9, 15, 1);
        draw_number(waveManager.getWave(), 5, 9, 15, 1);
    }

    drawSunHud();
    drawLivesHud();

    for (int i = 0; i < dmgPool.CAPACITY; i++) {
        DmgIndicator* di = dmgPool.get(i);
        if (!di->isActive()) continue;
        di->update();
        if (di->isActive()) di->render();
    }

    queue1.update();
    queue2.update();
    drawQueueHud(queue1, 2,   186, CURSOR_P1_COLOR);
    drawQueueHud(queue2, 200, 186, CURSOR_P2_COLOR);

    unsigned char* src = backbuffer;
    unsigned char* dst = real_video;
    for (int i = 0; i < 320 * 200; i++) dst[i] = src[i];
    video = real_video;
}

// ---------------------------------------------------------------------------
// Lives HUD
// ---------------------------------------------------------------------------

void PlantsVsZombies::drawLivesHud() {
    // Pixel-art heart, 5×4 cells, scale 2 → 10×8 px per heart
    static const bool heart[4][5] = {
        { 0, 1, 0, 1, 0 },
        { 1, 1, 1, 1, 1 },
        { 0, 1, 1, 1, 0 },
        { 0, 0, 1, 0, 0 },
    };
    const int scale  = 2;
    const int heartW = 5 * scale;
    const int gap    = 3;
    const int totalW = 3 * heartW + 2 * gap;
    const int startX = 320 - totalW - 1;
    const int startY = 16;

    for (int i = 0; i < 3; i++) {
        unsigned char color = (i < lives) ? (unsigned char)32 : (unsigned char)1;
        int ox = startX + i * (heartW + gap);
        for (int row = 0; row < 4; row++)
            for (int col = 0; col < 5; col++)
                if (heart[row][col])
                    plot_square(ox + col * scale, startY + row * scale, scale, color);
    }
}

// ---------------------------------------------------------------------------
// Game Over
// ---------------------------------------------------------------------------

void PlantsVsZombies::showGameOver() {
    for (int i = 0; i < 320 * 200; i++)
        video[i] = 0;

    // "GAME OVER" : 9 chars × (3+1) × scale3 = 108 px → centré à x=106
    draw_text("GAME OVER", 106, 85, 32, 3);

    // Temps de survie (secondes)
    int len = 0, tmp = lastSeconds;
    if (tmp == 0) len = 1; else { while (tmp > 0) { len++; tmp /= 10; } }
    int sx = (320 - (len + 1) * 8) / 2;   // chaque char = 4*scale2 = 8px
    draw_number(lastSeconds, sx, 112, 15, 2);
    draw_text("s", sx + len * 8, 112, 15, 2);

    // Vague atteinte
    draw_text("wave", 134, 126, 15, 2);
    draw_number(waveManager.getWave(), 134 + 4 * 8, 126, 15, 2);

    while (true) {}
}

// ---------------------------------------------------------------------------
// Input
// ---------------------------------------------------------------------------

/* [EXPLICATION] Architecture producteur / consommateur.
   Le handler IRQ1 (producteur) push() les événements dans keyboardQueue.
   handleInput() (consommateur) les pop() un par un.

   Avantage : chaque appui physique génère exactement UN événement make dans
   la file — pas de front montant à détecter, pas de problème typematique.
   Le ring buffer absorbe les rafales de touches pressées entre deux frames.

   Schéma de flux :
     IRQ1 (IF=0)     →  push(scanCode, pressed)  →  keyboardQueue (ring buffer)
     main loop (~62fps) →  while(pop(evt))         →  action curseur / placement

   La section critique (disable_IRQs dans pop) protège tryP() contre V() en IRQ :
   voir KeyboardQueue.cpp pour le détail.                                      */
void PlantsVsZombies::handleInput() {
    KeyEvent evt;
    while (keyboardQueue.pop(evt)) {
        if (!evt.pressed) continue;

        if      (evt.scanCode == SC_P1_UP)    cursorRow--;
        else if (evt.scanCode == SC_P1_DOWN)  cursorRow++;
        else if (evt.scanCode == SC_P1_LEFT)  cursorCol--;
        else if (evt.scanCode == SC_P1_RIGHT) cursorCol++;
        else if (evt.scanCode == SC_P1_PLACE) {
            int idx = queue1.getRosterCursor();
            if (idx >= queue1.getCount()) {
                queue1.triggerFlash();
            } else {
                PlantType type = queue1.getSlot(idx);
                if (!canAfford(PlantQueue::costOf(type))) {
                    sunFlashEndTick = compt + QUEUE_EMPTY_FLASH;
                    queue1.triggerFlash();
                } else if (placePlant(cursorCol, cursorRow, type)) {
                    PlantType taken;
                    queue1.tryTake(idx, taken);
                }
            }
        }
        /* Curseur roster P1 : W / X */
        else if (evt.scanCode == SC_P1_ROSTER_LEFT)  queue1.moveRosterLeft();
        else if (evt.scanCode == SC_P1_ROSTER_RIGHT) queue1.moveRosterRight();

        else if (evt.scanCode == SC_P2_UP)    cursorRow2--;
        else if (evt.scanCode == SC_P2_DOWN)  cursorRow2++;
        else if (evt.scanCode == SC_P2_LEFT)  cursorCol2--;
        else if (evt.scanCode == SC_P2_RIGHT) cursorCol2++;
        else if (evt.scanCode == SC_P2_PLACE) {
            int idx = queue2.getRosterCursor();
            if (idx >= queue2.getCount()) {
                queue2.triggerFlash();
            } else {
                PlantType type = queue2.getSlot(idx);
                if (!canAfford(PlantQueue::costOf(type))) {
                    sunFlashEndTick = compt + QUEUE_EMPTY_FLASH;
                    queue2.triggerFlash();
                } else if (placePlant(cursorCol2, cursorRow2, type)) {
                    PlantType taken;
                    queue2.tryTake(idx, taken);
                }
            }
        }
        /* Curseur roster P2 : Y / U */
        else if (evt.scanCode == SC_P2_ROSTER_LEFT)  queue2.moveRosterLeft();
        else if (evt.scanCode == SC_P2_ROSTER_RIGHT) queue2.moveRosterRight();
    }

    if (cursorCol  < 0)            cursorCol  = 0;
    if (cursorCol  >= Grid::COLS)  cursorCol  = Grid::COLS - 1;
    if (cursorRow  < 0)            cursorRow  = 0;
    if (cursorRow  >= Grid::ROWS)  cursorRow  = Grid::ROWS - 1;
    if (cursorCol2 < 0)            cursorCol2 = 0;
    if (cursorCol2 >= Grid::COLS)  cursorCol2 = Grid::COLS - 1;
    if (cursorRow2 < 0)            cursorRow2 = 0;
    if (cursorRow2 >= Grid::ROWS)  cursorRow2 = Grid::ROWS - 1;
}

// ---------------------------------------------------------------------------
// Placement
// ---------------------------------------------------------------------------

bool PlantsVsZombies::placePlant(int col, int row, PlantType type) {
    if (plantCount >= MAX_PLANTS) return false;

    int cost = PlantQueue::costOf(type);
    if (!canAfford(cost)) return false;

    if (grid.isTileOccupied(col, row)) return false;

    int px, py;
    grid.tileToPixel(col, row, px, py);

    Peashooter* p = 0;

    switch (type) {
        case PLANT_SNOW_PEASHOOTER:
            p = new SnowPeashooter(px, py);
            break;
        case PLANT_SUNFLOWER:
            p = new Sunflower(px, py);
            break;
        case PLANT_PEASHOOTER:
        default:
            p = new Peashooter(px, py);
            break;
    }

    if (p) {
        spendSuns(cost);
        plants[plantCount++] = p;
        Tile* t = grid.getTile(col, row);
        if (t) t->setState(TileState::HasPlant);
        return true;
    }
    return false;
}

// ---------------------------------------------------------------------------
// Cursor
// ---------------------------------------------------------------------------

void PlantsVsZombies::drawCursor(int col, int row, unsigned char color) {
    int px, py;
    grid.tileToPixel(col, row, px, py);
    int w = Grid::TILE_SIZE, h = Grid::TILE_SIZE;
    for (int x = px; x < px + w; x++) {
        video[py * 320 + x]           = color;
        video[(py + h - 1) * 320 + x] = color;
    }
    for (int y = py; y < py + h; y++) {
        video[y * 320 + px]           = color;
        video[y * 320 + (px + w - 1)] = color;
    }
}

/* Retourne le sprite (frame 0) correspondant à un PlantType. */
static const unsigned char* spriteForPlant(PlantType type, int& w, int& h) {
    switch (type) {
        case PLANT_SNOW_PEASHOOTER:
            w = SNOW_PEASHOOTER_WIDTH;
            h = SNOW_PEASHOOTER_HEIGHT;
            return snow_peashooter_frames[0];
        case PLANT_SUNFLOWER:
            w = SUNFLOWER_WIDTH;
            h = SUNFLOWER_HEIGHT;
            return sunflower_frames[0];
        case PLANT_PEASHOOTER:
        default:
            w = PEASHOOTER_WIDTH;
            h = PEASHOOTER_HEIGHT;
            return peashooter_frames[0];
    }
}

/* Affiche la file de plantes d'un joueur.
   px, py : coin supérieur gauche du HUD.
   color : couleur du curseur roster. */
void PlantsVsZombies::drawQueueHud(const PlantQueue& q, int px, int py, unsigned char color) {
    const int ICON_H  = 12;
    const int ICON_W  = 10;
    const int COST_H  =  6;
    const int SLOT_W  = ICON_W + 22;   // icon + espace + chiffres coût
    const int SLOT_H  = ICON_H + 2;    // icon + 1px padding haut/bas
    const int SLOT_GAP = 2;
    bool flash = q.isFlashing();

    for (int i = 0; i < QUEUE_CAPACITY; i++) {
        int sx = px + i * (SLOT_W + SLOT_GAP);

        /* Fond du slot */
        unsigned char bg = (i < q.getCount())
            ? (unsigned char)1
            : (flash ? (unsigned char)32 : (unsigned char)0);
        for (int r = 0; r < SLOT_H; r++)
            for (int c = 0; c < SLOT_W; c++)
                video[(py + r) * 320 + (sx + c)] = bg;

        if (i < q.getCount()) {
            /* Icône de plante (sprite mis à l'échelle) */
            int srcW, srcH;
            const unsigned char* spr = spriteForPlant(q.getSlot(i), srcW, srcH);
            draw_sprite_scaled(spr, srcW, srcH,
                               sx + 1, py + 1,
                               ICON_W, ICON_H);

            /* Coût en soleils à droite de l'icône */
            draw_number(PlantQueue::costOf(q.getSlot(i)),
                        sx + ICON_W + 2, py + (SLOT_H - COST_H) / 2,
                        SUN_HUD_YELLOW, 1);

            /* Surbrillance du slot sélectionné par le curseur roster */
            if (i == q.getRosterCursor()) {
                unsigned char borderCol = flash ? SUN_HUD_RED : color;
                for (int c = sx; c < sx + SLOT_W; c++) {
                    video[py * 320 + c]                = borderCol;
                    video[(py + SLOT_H - 1) * 320 + c] = borderCol;
                }
                for (int r = py; r < py + SLOT_H; r++) {
                    video[r * 320 + sx]                = borderCol;
                    video[r * 320 + (sx + SLOT_W - 1)] = borderCol;
                }
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Boucles des threads
// ---------------------------------------------------------------------------

/* [THREAD LOGIQUE] Tourne en Round-Robin coopératif.
   Toutes les 16 ms : lit les entrées clavier, met à jour l'état du jeu,
   puis signale le thread rendu via frameSem. Cède le CPU (Yield) après
   chaque itération pour laisser le thread rendu s'exécuter.

   Le compteur de ticks (compt) est incrémenté par l'IRQ timer (1000 Hz).
   On attend le prochain tick en cédant le CPU plutôt qu'en spinant
   (while + Yield), ce qui évite d'affamer le thread rendu pendant l'attente. */
void PlantsVsZombies::runLogic() {
    int lastTick     = compt;
    int fpsTimer     = compt;
    int renderFrames = 0;

    while (true) {
        /* Attente coopérative du prochain tick — Yield() plutôt que spin pur
           afin de laisser le thread rendu dessiner la frame précédente. */
        while (compt == lastTick) thread_yield();
        lastTick = compt;

        if ((compt % 16) == 0) {
            /* L'input est traité avant la logique pour que les actions du joueur
               (déplacement curseur, placement de plante) soient prises en compte
               dans la frame courante. */
            handleInput();
            updateLogic();

            /* Signal au thread rendu : une nouvelle frame est prête. */
            frameSem.V();
            renderFrames++;
        }

        /* Mise à jour du compteur FPS une fois par seconde. */
        if (compt - fpsTimer >= 1000) {
            lastFps      = renderFrames;
            lastSeconds  = compt / 1000;
            renderFrames = 0;
            fpsTimer     = compt;
        }

        /* Fin de partie : showGameOver() ne retourne jamais (boucle infinie). */
        if (lives <= 0) showGameOver();

        /* Cède le CPU au thread rendu (et éventuellement au thread idle). */
        thread_yield();
    }
}

/* [THREAD RENDU] Tourne en Round-Robin coopératif.
   Attend le signal de runLogic() via frameSem (tryP non bloquant + Yield),
   puis compose le backbuffer et le copie dans le framebuffer VGA. */
void PlantsVsZombies::runRender() {
    while (true) {
        /* tryP() non bloquant : si aucune frame logique n'est prête, on cède
           le CPU immédiatement plutôt que de spinner (important en coopératif :
           le spinning empêcherait runLogic de s'exécuter). */
        while (!frameSem.tryP()) thread_yield();

        renderFrame();

        /* Cède le CPU après chaque frame rendue. */
        thread_yield();
    }
}

// ---------------------------------------------------------------------------
// Démarrage — crée les threads et devient thread idle
// ---------------------------------------------------------------------------

void PlantsVsZombies::start() {
    /* Pré-remplir les files des joueurs avec 2 plantes chacune. */
    queue1.seed(2);
    queue2.seed(2);

    /* [THREADS] Création des threads de jeu.
       Les structures sont statiques : elles persistent au-delà du retour de
       start() (qui ne se produit jamais en pratique), et leur durée de vie
       est celle du programme.

       Ordre de création : logicThread en premier, renderThread en second.
       L'ordonnanceur Round-Robin va tourner : init → logicThread → renderThread
       → init (idle) → logicThread → … */
    static PvZLogicThread  logicThread;
    static PvZRenderThread renderThread;

    logicThread.game  = this;
    renderThread.game = this;

    logicThread.start();
    renderThread.start();

    /* Le thread initial (celui qui a exécuté main()) devient un thread idle :
       il cède le CPU indéfiniment. Les threads de jeu ne terminent jamais
       (showGameOver() boucle à l'infini), donc ce thread idle ne reprend
       jamais la main en pratique. */
    while (true) thread_yield();
}
