/*
 * input_handler.cpp
 * Implements InputHandler — processes arrow keys, WASD, and mouse clicks.
 * As a friend of Game, it can directly read and modify private game state.
 */

#include "input_handler.h"
#include "../core/game.h"
#include "../gui/renderer.h"

// Handles keyboard input: arrow keys/WASD for movement, C to continue after winning
bool InputHandler::handleKeyDown(Game& game, WPARAM key, HWND hwnd) {
    if (game.m_won && !game.m_keepPlaying) {
        if (key == 'C') {
            game.m_keepPlaying = true;
            InvalidateRect(hwnd, NULL, FALSE);
        }
        return true;
    }

    if (game.m_gameOver) return false;

    bool moved = false;
    switch (key) {
        case VK_UP:    case 'W': moved = game.moveUp();    break;
        case VK_DOWN:  case 'S': moved = game.moveDown();  break;
        case VK_LEFT:  case 'A': moved = game.moveLeft();  break;
        case VK_RIGHT: case 'D': moved = game.moveRight(); break;
    }

    if (moved) {
        game.processMove(moved, hwnd);
    }

    return moved;
}

// Handles mouse clicks — checks if the "New Game" button was clicked
bool InputHandler::handleMouseClick(Game& game, int x, int y, HWND hwnd) {
    if (game.m_renderer) {
        RECT btn = game.m_renderer->getButtonRect();
        POINT pt = { x, y };
        if (PtInRect(&btn, pt)) {
            game.startNewGame();
            InvalidateRect(hwnd, NULL, FALSE);
            return true;
        }
    }
    return false;
}
