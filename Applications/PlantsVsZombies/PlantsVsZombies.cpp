#include <Applications/PlantsVsZombies/PlantsVsZombies.h>
#include <Applications/PlantsVsZombies/PeashooterBullet.h>
#include <Applications/PlantsVsZombies/SnowPeashooter.h>
#include <Applications/PlantsVsZombies/sprites/shared_palette.h>
#include <Applications/PlantsVsZombies/sprites/peashooter_sprite.h>
#include <Applications/PlantsVsZombies/sprites/snow_peashooter_sprite.h>
#include <Applications/PlantsVsZombies/sprites/zombie_walk_sprite.h>
#include <sextant/interruptions/handler/handler_clavier.h>
#include <sextant/memoire/memoire.h>
#include <vga/vga.h>

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
    plot_square(x + scale, y,          scale, color); // haut
    plot_square(x,         y + scale,  scale, color); // gauche
    plot_square(x + scale, y + scale,  scale, color); // centre
    plot_square(x + scale*2, y + scale,scale, color); // droite
    plot_square(x + scale, y + scale*2,scale, color); // bas
}

void PlantsVsZombies::drawSunHud() {
    // Panneau centré : x=120, y=1, 80×20 px
    const int px = 120, py = 1, pw = 80, ph = 20;
    for (int row = 0; row < ph; row++)
        for (int col = 0; col < pw; col++)
            video[(py + row) * 320 + (px + col)] = SUN_HUD_BG;

    // Nombre de soleils (rouge si pas assez)
    bool sunFlash = compt < sunFlashEndTick;
    unsigned char sunColor = sunFlash ? SUN_HUD_RED : SUN_HUD_TEXT;
    unsigned char borderColor = sunFlash ? SUN_HUD_RED : (unsigned char)3;

    // Bordures pour délimiter le panneau
    for (int col = 0; col < pw; col++) {
        video[py * 320 + (px + col)]            = borderColor;
        video[(py + ph - 1) * 320 + (px + col)] = borderColor;
    }

    // Icône soleil placeholder 14×14
    plot_square(px + 4, py + 3, 14, SUN_HUD_YELLOW);

    // Nombre de soleils
    draw_number(suns, px + 22, py + 6, sunColor, 2);

    // Affichage "+Y" pendant SUN_DISPLAY_DURATION ticks après un gain
    if (compt < sunGainDisplayEnd) {
        draw_plus(px + 52, py + 6, SUN_HUD_GREEN, 2);
        draw_number(SUN_TICK_AMOUNT, px + 60, py + 6, SUN_HUD_GREEN, 2);
    }
}
/* Couleurs des curseurs (indices dans shared_palette).
   P1 = blanc (index 15), P2 = vert (index 205, même teinte que la barre de vie). */
#define CURSOR_P1_COLOR 15
#define CURSOR_P2_COLOR 205

PlantsVsZombies::PlantsVsZombies() : plantCount(0), bulletCount(0), zombieCount(0), dmgIndicatorCount(0) {
    for (int i = 0; i < MAX_PLANTS; i++)
        plants[i] = 0;
    for (int i = 0; i < MAX_BULLETS; i++)
        bullets[i] = 0;
    for (int i = 0; i < MAX_ZOMBIES; i++)
        zombies[i] = 0;
}

void PlantsVsZombies::init(Ecran* e,Clavier* c) {
    ecran = e;
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

void PlantsVsZombies::update_screen() {

    // --- Gain automatique de soleils ---
    if (compt - lastSunTick >= SUN_TICK_INTERVAL) {
        addSuns(SUN_TICK_AMOUNT);
        lastSunTick       = compt;
        sunGainDisplayEnd = compt + SUN_DISPLAY_DURATION;
    }

    // --- Plants: update + spawn bullets + remove dead ---
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
        if (plants[i]->canShoot() && bulletCount < MAX_BULLETS) {
            int bx = plants[i]->getX();
            int by = plants[i]->getY() + plants[i]->getHeight() / 4;
            Bullet* b = plants[i]->createBullet(bx, by);
            if (b) {
                bullets[bulletCount++] = b;
                plants[i]->resetCooldown();
            }
        }
    }

    // --- Zombies: block/unblock + damage plant + update + remove dead ---
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
        if (zombies[i]->isDead()) {
            delete zombies[i];
            zombies[i] = zombies[--zombieCount];
            zombies[zombieCount] = 0;
            i--;
        }
    }

    // --- Bullets: update + collision vs zombies + remove inactive ---
    for (int i = 0; i < bulletCount; i++) {
        bullets[i]->update();
        if (bullets[i]->isActive()
                && bullets[i]->getX() > bullets[i]->getSpawnX() + COLLISION_DISTANCE) {
            for (int z = 0; z < zombieCount; z++) {
                if (aabb(bullets[i]->getX(),  bullets[i]->getY(),
                         bullets[i]->getWidth(), bullets[i]->getHeight(),
                         zombies[z]->getX(),  zombies[z]->getY(),
                         zombies[z]->getWidth(), zombies[z]->getHeight())) {
                    int dmg = bullets[i]->getDamage();
                    bullets[i]->onHit(*zombies[z]);
                    bullets[i]->deactivate();
                    /* Damage indicator */
                    if (dmgIndicatorCount < MAX_DMG_INDICATORS) {
                        dmgIndicators[dmgIndicatorCount].x = zombies[z]->getX();
                        dmgIndicators[dmgIndicatorCount].y = zombies[z]->getY() - 4;
                        dmgIndicators[dmgIndicatorCount].value = dmg;
                        dmgIndicators[dmgIndicatorCount].endTick = compt + DMG_INDICATOR_DURATION;
                        dmgIndicatorCount++;
                    }
                    break;
                }
            }
        }
        if (!bullets[i]->isActive()) {
            delete bullets[i];
            bullets[i] = bullets[--bulletCount];
            bullets[bulletCount] = 0;
            i--;
        }
    }

    // Rendering
    unsigned char* real_video = (unsigned char*) video;
    video = backbuffer;

    clear_vga_screen(0); 
    grid.render();

    for (int i = 0; i < plantCount; i++)
        if (plants[i]) plants[i]->render();

    for (int i = 0; i < bulletCount; i++)
        if (bullets[i]) bullets[i]->render();

    for (int i = 0; i < zombieCount; i++)
        if (zombies[i]) zombies[i]->render();
        
    unsigned char c1 = grid.isTileOccupied(cursorCol, cursorRow)   ? SUN_HUD_RED : CURSOR_P1_COLOR;
    unsigned char c2 = grid.isTileOccupied(cursorCol2, cursorRow2) ? SUN_HUD_RED : CURSOR_P2_COLOR;
    drawCursor(cursorCol,  cursorRow,  c1);
    drawCursor(cursorCol2, cursorRow2, c2);

    // compteurs
    draw_number(lastSeconds, 0, 2, 15, 2);
    draw_number(lastFps, 290, 2, 15, 2);

    drawSunHud();

    /* Damage indicators */
    for (int i = 0; i < dmgIndicatorCount; i++) {
        if (compt < dmgIndicators[i].endTick) {
            int elapsed = DMG_INDICATOR_DURATION - (dmgIndicators[i].endTick - compt);
            int floatY  = dmgIndicators[i].y - (elapsed / 50);
            draw_number(dmgIndicators[i].value, dmgIndicators[i].x, floatY, SUN_HUD_YELLOW, 1);
        } else {
            dmgIndicatorCount--;
            if (i < dmgIndicatorCount) {
                dmgIndicators[i] = dmgIndicators[dmgIndicatorCount];
            }
            i--;
        }
    }

    /* Mise à jour et affichage des files de plantes */
    queue1.update();
    queue2.update();
    drawQueueHud(queue1, 2,  186, CURSOR_P1_COLOR);
    drawQueueHud(queue2, 200, 186, CURSOR_P2_COLOR);

    unsigned char* src = backbuffer;
    unsigned char* dst = real_video;
    for (int i = 0; i < 320 * 200; i++)
        dst[i] = src[i];
    video = real_video;
}

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

    /* [EXPLICATION] On draine TOUS les événements accumulés depuis le dernier frame.
       Si plusieurs touches ont été pressées pendant les ~16 ms inter-frames, elles
       sont toutes traitées ici, dans l'ordre d'arrivée.                           */
    while (keyboardQueue.pop(evt)) {
        if (!evt.pressed) continue; /* on ignore les break (relâchement) */

        /* --- Joueur 1 : ZQSD + Espace --- */
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

        /* --- Joueur 2 : IJKL + O --- */
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

    /* Borne les curseurs dans les limites de la grille. */
    if (cursorCol < 0)            cursorCol = 0;
    if (cursorCol >= Grid::COLS)  cursorCol = Grid::COLS - 1;
    if (cursorRow < 0)            cursorRow = 0;
    if (cursorRow >= Grid::ROWS)  cursorRow = Grid::ROWS - 1;

    if (cursorCol2 < 0)           cursorCol2 = 0;
    if (cursorCol2 >= Grid::COLS) cursorCol2 = Grid::COLS - 1;
    if (cursorRow2 < 0)           cursorRow2 = 0;
    if (cursorRow2 >= Grid::ROWS) cursorRow2 = Grid::ROWS - 1;
}

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

void PlantsVsZombies::drawCursor(int col, int row, unsigned char color) {
    int px, py;
    grid.tileToPixel(col, row, px, py);

    int w = Grid::TILE_SIZE;
    int h = Grid::TILE_SIZE;

    /* Bordure supérieure et inférieure. */
    for (int x = px; x < px + w; x++) {
        video[py * 320 + x]           = color;
        video[(py + h - 1) * 320 + x] = color;
    }
    /* Bordure gauche et droite. */
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
            return snow_peashooter_sprite_data;
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
                    video[py * 320 + c]                  = borderCol;
                    video[(py + SLOT_H - 1) * 320 + c]   = borderCol;
                }
                for (int r = py; r < py + SLOT_H; r++) {
                    video[r * 320 + sx]                  = borderCol;
                    video[r * 320 + (sx + SLOT_W - 1)]   = borderCol;
                }
            }
        }
    }
}

void PlantsVsZombies::start() {
    /* Zombie spawns at right edge of screen, grid-aligned to row 0 */
    int zy = Grid::OFFSET_Y + 0 * Grid::TILE_SIZE;
    zombies[0] = new Zombie(320 - ZOMBIE_WALK_WIDTH, zy);
    zombieCount = 1;

    /* Pré-remplir les files des joueurs avec 2 plantes chacune. */
    queue1.seed(2);
    queue2.seed(2);

    lastSunTick = compt; // premier gain après SUN_TICK_INTERVAL depuis le lancement

    int lastTick  = compt;
    int renderFrames = 0;
    int fpsTimer  = compt;

    while (true) {
        while (compt == lastTick);
        lastTick = compt;

        if ((compt % 16) == 0) {
            /* [EXPLICATION] handleInput() draine la file d'événements clavier accumulés
               depuis le dernier frame (~16 ms).  Grâce à l'architecture producteur/
               consommateur, chaque appui physique génère exactement un événement make :
               pas de répétition typematique, pas de front montant à détecter.         */
            handleInput();
            update_screen();
            renderFrames++;
        }

        if (compt - fpsTimer >= 1000) {
            lastFps      = renderFrames;
            lastSeconds  = compt / 1000;
            renderFrames = 0;
            fpsTimer     = compt;
        }
    }
}
