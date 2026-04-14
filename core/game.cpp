/*
 * game.cpp
 * Implements the Game class — initialization, move processing, state management,
 * and the friend debug function.
 */

#include "game.h"
#include "../gui/renderer.h"

// Initializes both base classes (Board and ScoreManager) and game state
Game::Game()
    : Board(),
      ScoreManager(),
      m_won(false),
      m_keepPlaying(false),
      m_gameOver(false),
      m_renderer(nullptr)
{
}

Game::~Game() {}

// Resets both the board and the score, and clears game state flags
void Game::reset() {
    Board::reset();
    ScoreManager::reset();
    m_won = false;
    m_keepPlaying = false;
    m_gameOver = false;
}

// Returns true if the game is over (no valid moves remaining)
bool Game::isGameOver() const {
    return m_gameOver;
}

// Starts a new game by resetting and spawning two initial tiles
void Game::startNewGame() {
    reset();
    spawnTile();
    spawnTile();
}

// Called after a move — spawns a new tile, updates scores, checks for game over
void Game::processMove(bool moved, HWND hwnd) {
    if (moved) {
        spawnTile();
        ScoreManager::updateBestScore(getCurrentScore());
        if (!canMove())
            m_gameOver = true;
        if (hasWinTile())
            m_won = true;
        InvalidateRect(hwnd, NULL, FALSE);
    }
}

bool Game::hasWon() const { return m_won; }
bool Game::isKeepPlaying() const { return m_keepPlaying; }
void Game::continueAfterWin() { m_keepPlaying = true; }
bool Game::isOver() const { return m_gameOver; }
int Game::getScore() const { return getCurrentScore(); }

GDIRenderer* Game::getRenderer() { return m_renderer; }
void Game::setRenderer(GDIRenderer* r) { m_renderer = r; }

// Friend function: can access Game's private members for debugging
void printGameDebugInfo(const Game& game) {
    OutputDebugStringA(game.m_won ? "Game: WON\n" : "Game: NOT WON\n");
    OutputDebugStringA(game.m_gameOver ? "Game: OVER\n" : "Game: ACTIVE\n");
}
