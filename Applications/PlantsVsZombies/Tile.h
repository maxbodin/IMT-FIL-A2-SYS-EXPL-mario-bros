#ifndef TILE_H
#define TILE_H

enum class TileState {
    Empty,
    HasPlant,
    HasZombie,
    Contested,
};

class Tile {
public:
    Tile() = default;
    Tile(int col, int row);

    int getCol() const { return m_col; }
    int getRow() const { return m_row; }

    TileState getState() const { return m_state; }
    void setState(TileState s) { m_state = s; m_dirty = true; }

    void markDirty() { m_dirty = true; }

    void render(int px, int py);

private:
    int m_col { 0 };
    int m_row { 0 };
    TileState m_state { TileState::Empty };
    bool m_dirty { true };
};

#endif