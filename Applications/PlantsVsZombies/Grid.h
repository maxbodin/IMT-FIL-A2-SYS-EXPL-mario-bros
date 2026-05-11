#ifndef GRID_H
#define GRID_H

#include <Applications/PlantsVsZombies/Tile.h>

class Grid {
public:
    static const int ROWS = 5;
    static const int COLS = 9;
    static const int TILE_SIZE = 32;
    static const int OFFSET_X = 16;
    static const int OFFSET_Y = 24;
    Tile tiles[ROWS][COLS];

    Grid();

    Tile* getTile(int col, int row);
    void tileToPixel(int col, int row, int& px, int& py);
    bool pixelToTile(int px, int py, int& col, int& row);
    bool isTileOccupied(int col, int row);
    void render();
    void renderTile(int col, int row);
};

#endif
