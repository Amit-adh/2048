/*
 * board.cpp
 * Implements Board class — tile movement, merging, spawning, and game state.
 */

#include "board.h"
#include <cstdlib>

// Initializes an empty board
Board::Board() : IGameEntity(), m_hasWinTile(false) {
    reset();
}

Board::~Board() {}

// Clears all tiles on the board
void Board::reset() {
    for (int i = 0; i < GRID; i++)
        for (int j = 0; j < GRID; j++)
            m_grid[i][j].setValue(0);
    m_hasWinTile = false;
}

// Returns true if no moves are possible
bool Board::isGameOver() const {
    return !canMove();
}

// Slides and merges tiles to the left, returns true if board changed
bool Board::moveLeft() {
    bool moved = false;
    for (int i = 0; i < GRID; i++) {
        int merged[GRID] = {};
        for (int j = 1; j < GRID; j++) {
            if (m_grid[i][j].isEmpty()) continue;
            int k = j;
            while (k > 0 && m_grid[i][k-1].isEmpty()) {
                m_grid[i][k-1] = m_grid[i][k];
                m_grid[i][k].setValue(0);
                k--; moved = true;
            }
            if (k > 0 && m_grid[i][k-1] == m_grid[i][k] && !merged[k-1]) {
                Tile combined = m_grid[i][k-1] + m_grid[i][k];
                m_grid[i][k-1] = combined;
                m_grid[i][k].setValue(0);
                merged[k-1] = 1; moved = true;
                if (m_grid[i][k-1].getValue() == WIN_VALUE) m_hasWinTile = true;
            }
        }
    }
    return moved;
}

// Slides and merges tiles to the right, returns true if board changed
bool Board::moveRight() {
    bool moved = false;
    for (int i = 0; i < GRID; i++) {
        int merged[GRID] = {};
        for (int j = GRID - 2; j >= 0; j--) {
            if (m_grid[i][j].isEmpty()) continue;
            int k = j;
            while (k < GRID - 1 && m_grid[i][k+1].isEmpty()) {
                m_grid[i][k+1] = m_grid[i][k];
                m_grid[i][k].setValue(0);
                k++; moved = true;
            }
            if (k < GRID - 1 && m_grid[i][k+1] == m_grid[i][k] && !merged[k+1]) {
                Tile combined = m_grid[i][k+1] + m_grid[i][k];
                m_grid[i][k+1] = combined;
                m_grid[i][k].setValue(0);
                merged[k+1] = 1; moved = true;
                if (m_grid[i][k+1].getValue() == WIN_VALUE) m_hasWinTile = true;
            }
        }
    }
    return moved;
}

// Slides and merges tiles upward, returns true if board changed
bool Board::moveUp() {
    bool moved = false;
    for (int j = 0; j < GRID; j++) {
        int merged[GRID] = {};
        for (int i = 1; i < GRID; i++) {
            if (m_grid[i][j].isEmpty()) continue;
            int k = i;
            while (k > 0 && m_grid[k-1][j].isEmpty()) {
                m_grid[k-1][j] = m_grid[k][j];
                m_grid[k][j].setValue(0);
                k--; moved = true;
            }
            if (k > 0 && m_grid[k-1][j] == m_grid[k][j] && !merged[k-1]) {
                Tile combined = m_grid[k-1][j] + m_grid[k][j];
                m_grid[k-1][j] = combined;
                m_grid[k][j].setValue(0);
                merged[k-1] = 1; moved = true;
                if (m_grid[k-1][j].getValue() == WIN_VALUE) m_hasWinTile = true;
            }
        }
    }
    return moved;
}

// Slides and merges tiles downward, returns true if board changed
bool Board::moveDown() {
    bool moved = false;
    for (int j = 0; j < GRID; j++) {
        int merged[GRID] = {};
        for (int i = GRID - 2; i >= 0; i--) {
            if (m_grid[i][j].isEmpty()) continue;
            int k = i;
            while (k < GRID - 1 && m_grid[k+1][j].isEmpty()) {
                m_grid[k+1][j] = m_grid[k][j];
                m_grid[k][j].setValue(0);
                k++; moved = true;
            }
            if (k < GRID - 1 && m_grid[k+1][j] == m_grid[k][j] && !merged[k+1]) {
                Tile combined = m_grid[k+1][j] + m_grid[k][j];
                m_grid[k+1][j] = combined;
                m_grid[k][j].setValue(0);
                merged[k+1] = 1; moved = true;
                if (m_grid[k+1][j].getValue() == WIN_VALUE) m_hasWinTile = true;
            }
        }
    }
    return moved;
}

// Places a new tile (90% chance of 2, 10% chance of 4) on a random empty cell
bool Board::spawnTile() {
    std::vector<std::pair<int,int>> empty;
    for (int i = 0; i < GRID; i++)
        for (int j = 0; j < GRID; j++)
            if (m_grid[i][j].isEmpty())
                empty.push_back({i, j});
    if (empty.empty()) return false;
    int idx = rand() % empty.size();
    int val = (rand() % 10 < 9) ? 2 : 4;
    m_grid[empty[idx].first][empty[idx].second].setValue(val);
    return true;
}

// Returns true if any move is possible (empty cell or adjacent equal tiles)
bool Board::canMove() const {
    for (int i = 0; i < GRID; i++)
        for (int j = 0; j < GRID; j++) {
            if (m_grid[i][j].isEmpty()) return true;
            if (j < GRID - 1 && m_grid[i][j] == m_grid[i][j+1]) return true;
            if (i < GRID - 1 && m_grid[i][j] == m_grid[i+1][j]) return true;
        }
    return false;
}

// Returns the tile at the given row and column
const Tile& Board::getTile(int row, int col) const {
    return m_grid[row][col];
}

// Returns true if a tile with WIN_VALUE has been created
bool Board::hasWinTile() const {
    return m_hasWinTile;
}
