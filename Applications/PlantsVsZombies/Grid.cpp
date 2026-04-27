#include <Applications/PlantsVsZombies/Grid.h>
#include <vga/vga.h>

Grid::Grid() {
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            tiles[row][col] = Tile(col, row);
        }
    }
}

Tile* Grid::getTile(int col, int row) {
    if (col < 0 || col >= COLS || row < 0 || row >= ROWS)
        return 0;
    return &tiles[row][col];
}

void Grid::tileToPixel(int col, int row, int& px, int& py) {
    px = OFFSET_X + col * TILE_SIZE;
    py = OFFSET_Y + row * TILE_SIZE;
}

void Grid::render() {
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            int px, py;
            tileToPixel(col, row, px, py);
            tiles[row][col].render(px, py);
        }
    }
}
