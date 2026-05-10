#include <Applications/PlantsVsZombies/PlantsVsZombies.h>
#include <Applications/PlantsVsZombies/PeashooterBullet.h>
#include <Applications/PlantsVsZombies/sprites/shared_palette.h>
#include <Applications/PlantsVsZombies/sprites/peashooter_sprite.h>
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

    // Bordures pour délimiter le panneau
    for (int col = 0; col < pw; col++) {
        video[py * 320 + (px + col)]            = 3;
        video[(py + ph - 1) * 320 + (px + col)] = 3;
    }

    // Icône soleil placeholder 14×14 (sera remplacée par draw_sprite)
    plot_square(px + 4, py + 3, 14, SUN_HUD_YELLOW);

    // Nombre de soleils
    draw_number(suns, px + 22, py + 6, SUN_HUD_TEXT, 2);

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

PlantsVsZombies::PlantsVsZombies() : plantCount(0), bulletCount(0), zombieCount(0) {
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
            delete plants[i];
            plants[i] = plants[--plantCount];
            plants[plantCount] = 0;
            i--;
            continue;
        }
        if (plants[i]->canShoot() && bulletCount < MAX_BULLETS) {
            int bx = plants[i]->getX();
            int by = plants[i]->getY() + plants[i]->getHeight() / 4;
            PeashooterBullet* b = new PeashooterBullet(bx, by);
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
                    zombies[z]->takeDamage(bullets[i]->getDamage());
                    bullets[i]->deactivate();
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

    drawCursor(cursorCol,  cursorRow,  CURSOR_P1_COLOR);
    drawCursor(cursorCol2, cursorRow2, CURSOR_P2_COLOR);

    // compteurs
    draw_number(lastSeconds, 0, 2, 15, 2);
    draw_number(lastFps, 290, 2, 15, 2);

    drawSunHud();

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
        else if (evt.scanCode == SC_P1_PLACE) placePlant(cursorCol, cursorRow);

        /* --- Joueur 2 : IJKL + O --- */
        else if (evt.scanCode == SC_P2_UP)    cursorRow2--;
        else if (evt.scanCode == SC_P2_DOWN)  cursorRow2++;
        else if (evt.scanCode == SC_P2_LEFT)  cursorCol2--;
        else if (evt.scanCode == SC_P2_RIGHT) cursorCol2++;
        else if (evt.scanCode == SC_P2_PLACE) placePlant(cursorCol2, cursorRow2);
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

void PlantsVsZombies::placePlant(int col, int row) {
    if (plantCount >= MAX_PLANTS) return;

    /* [EXPLICATION] Vérification d'occupation de la case : on parcourt le tableau
       des plantes existantes.  Pas besoin de section critique ici — placePlant
       est toujours appelé depuis handleInput() dans la boucle principale,
       donc en dehors de tout contexte d'interruption.  Les plantes ne sont
       jamais créées ni détruites par un handler IRQ.                          */
    int px, py;
    grid.tileToPixel(col, row, px, py);
    for (int i = 0; i < plantCount; i++) {
        if (plants[i] && plants[i]->getX() == px && plants[i]->getY() == py)
            return;
    }

    Peashooter* p = new Peashooter(px, py);
    if (p) plants[plantCount++] = p;
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

void PlantsVsZombies::start() {
    int zx, zy;
    grid.tileToPixel(8, 0, zx, zy);
    zombies[0] = new Zombie(zx, zy);
    zombieCount = 1;

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
