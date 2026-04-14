/*
 * board.h
 * Declares the Board class which manages the 4x4 tile grid.
 * Handles tile movement, merging, spawning, and win/loss detection.
 * Inherits from IGameEntity.
 */

#pragma once

#include "game_entity.h"
#include "tile.h"
#include <vector>
#include <utility>

class InputHandler;

class Board : public IGameEntity {
public:
    static const int GRID = 4;
    static const int WIN_VALUE = 2048;

protected:
    Tile m_grid[GRID][GRID];
    bool m_hasWinTile;

public:
    Board();
    virtual ~Board();

    void reset() override;
    bool isGameOver() const override;

    bool moveLeft();
    bool moveRight();
    bool moveUp();
    bool moveDown();
    bool spawnTile();
    bool canMove() const;

    const Tile& getTile(int row, int col) const;
    bool hasWinTile() const;

    friend class InputHandler;
};
