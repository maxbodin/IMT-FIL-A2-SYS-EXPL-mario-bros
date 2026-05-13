#include <Applications/PlantsVsZombies/Sun.h>
#include <Applications/PlantsVsZombies/Grid.h>
#include <Applications/PlantsVsZombies/sprites/objects/sun_big_sprite.h>
#include <vga/vga.h>

extern volatile int compt;

static unsigned int sun_rng = 99999;
static unsigned int sun_lcg() {
    sun_rng = sun_rng * 1103515245 + 12345;
    return (sun_rng >> 16) & 0x7FFF;
}

Sun::Sun(int x, int y)
    : x(x), y(y), frame(0), animTick(0), active(true)
{
    sun_rng ^= (unsigned int)compt;
    // Offset the sun position slightly randomly around the plant feet
    int offsetX = (int)(sun_lcg() % 13) - 6;  // -6 to +6
    int offsetY = (int)(sun_lcg() % 5) - 2;   // -2 to +2 (near feet level)
    this->x = x + offsetX;
    this->y = y + offsetY;

    // Compute tile position
    tileCol = (this->x - Grid::OFFSET_X) / Grid::TILE_SIZE;
    tileRow = (this->y - Grid::OFFSET_Y) / Grid::TILE_SIZE;
    if (tileCol < 0) tileCol = 0;
    if (tileCol >= Grid::COLS) tileCol = Grid::COLS - 1;
    if (tileRow < 0) tileRow = 0;
    if (tileRow >= Grid::ROWS) tileRow = Grid::ROWS - 1;

    value = randomValue();
    spawnTick = compt;
}

int Sun::randomValue() {
    return SUN_COLLECT_MIN + (int)(sun_lcg() % (SUN_COLLECT_MAX - SUN_COLLECT_MIN + 1));
}

void Sun::update() {
    if (!active) return;
    if (compt - spawnTick >= SUN_LIFETIME) {
        active = false;
        return;
    }
    if (++animTick >= ANIM_SPEED) {
        animTick = 0;
        frame = (frame + 1) % SUN_BIG_FRAMES;
    }
}

void Sun::render() {
    if (!active) return;
    draw_sprite_alpha(sun_big_frames[frame], SUN_BIG_WIDTH, SUN_BIG_HEIGHT, x, y, 179);
}

bool Sun::isActive()    const { return active; }
void Sun::collect()           { active = false; }
int  Sun::getX()        const { return x; }
int  Sun::getY()        const { return y; }
int  Sun::getTileCol()  const { return tileCol; }
int  Sun::getTileRow()  const { return tileRow; }
int  Sun::getValue()    const { return value; }
