#include <Applications/PlantsVsZombies/Grid.h>
#include <Applications/PlantsVsZombies/Tile.h>
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

bool Grid::pixelToTile(int px, int py, int& col, int& row) {
    col = (px - OFFSET_X) / TILE_SIZE;
    row = (py - OFFSET_Y) / TILE_SIZE;
    return col >= 0 && col < COLS && row >= 0 && row < ROWS;
}

bool Grid::isTileOccupied(int col, int row) {
    Tile* t = getTile(col, row);
    return t && t->getState() != TileState::Empty;
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

void Grid::renderTile(int col, int row) {
    if (col < 0 || col >= COLS || row < 0 || row >= ROWS) return;
    int px, py;
    tileToPixel(col, row, px, py);
    tiles[row][col].render(px, py);
}
