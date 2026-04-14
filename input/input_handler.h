/*
 * input_handler.h
 * Declares InputHandler which processes keyboard and mouse input for the game.
 * Declared as a friend class of Game to directly access game state.
 */

#pragma once

#include <windows.h>

class Game;

class InputHandler {
public:
    static bool handleKeyDown(Game& game, WPARAM key, HWND hwnd);
    static bool handleMouseClick(Game& game, int x, int y, HWND hwnd);
};
