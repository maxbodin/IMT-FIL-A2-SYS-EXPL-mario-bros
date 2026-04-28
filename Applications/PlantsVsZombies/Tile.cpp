#include <Applications/PlantsVsZombies/Tile.h>
#include <vga/vga.h>
#include <Applications/PlantsVsZombies/sprites/grass_tile_sprite.h>

Tile::Tile(int col, int row)
    : m_col(col), m_row(row)
{}

void Tile::render(int px, int py) {
    draw_sprite(grass_tile_sprite_data, GRASS_TILE_WIDTH, GRASS_TILE_HEIGHT, px, py);
}