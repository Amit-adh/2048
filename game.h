/*
 * game.h
 * Declares the Game class which combines board logic and score management.
 * Inherits from both Board and ScoreManager (multiple inheritance).
 * Manages win/loss state, renderer reference, and game flow.
 */

#pragma once

#include <windows.h>
#include "board.h"
#include "score_manager.h"

class GDIRenderer;
class InputHandler;
class Game;

void printGameDebugInfo(const Game& game);

class Game : public Board, public ScoreManager {
private:
    bool m_won;
    bool m_keepPlaying;
    bool m_gameOver;
    GDIRenderer* m_renderer;

public:
    Game();
    ~Game();

    void reset() override;
    bool isGameOver() const override;

    void startNewGame();
    void processMove(bool moved, HWND hwnd);

    bool hasWon() const;
    bool isKeepPlaying() const;
    void continueAfterWin();
    bool isOver() const;
    int getScore() const;

    GDIRenderer* getRenderer();
    void setRenderer(GDIRenderer* r);

    friend class InputHandler;
    friend void printGameDebugInfo(const Game& game);
};
