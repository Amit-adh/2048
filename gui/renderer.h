/*
 * renderer.h
 * Declares the abstract IRenderer interface and GDIRenderer implementation.
 * GDIRenderer handles all GDI+ drawing — tiles, scores, overlays, and buttons.
 * Also defines rendering constants, color palette, and tile theme data.
 */

#pragma once

#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>
#include <gdiplus.h>
#include "../core/tile.h"

using namespace Gdiplus;

static const int TILE_SIZE = 107;
static const int TILE_GAP = 12;
static const int BOARD_PADDING = 12;
static const int BOARD_Y_OFFSET = 140;
static const int GRID_DIM = 4;

static const int BOARD_RENDER_SIZE = BOARD_PADDING * 2 + GRID_DIM * TILE_SIZE + (GRID_DIM - 1) * TILE_GAP;
static const int WINDOW_W = BOARD_RENDER_SIZE + 40;
static const int WINDOW_H = BOARD_Y_OFFSET + BOARD_RENDER_SIZE + 40;

static const ARGB CLR_BG       = 0xFFFAF8EF;
static const ARGB CLR_BOARD_BG = 0xFFBBADA0;
static const ARGB CLR_DARK     = 0xFF776E65;
static const ARGB CLR_LIGHT    = 0xFFF9F6F2;
static const ARGB CLR_SCORE_BG = 0xFFBBADA0;
static const ARGB CLR_BTN      = 0xFF8F7A66;
static const ARGB CLR_OVERLAY  = 0xBBFAF8EF;
static const ARGB CLR_LABEL    = 0xFFEEE4DA;
static const ARGB CLR_SUBTITLE = 0xFFA0968B;

struct TileTheme {
    ARGB bg;
    ARGB fg;
    float fontSize;
};

TileTheme getTileTheme(int value);

class Game;

class IRenderer {
public:
    virtual void renderFrame(HDC hdc, HWND hwnd) = 0;
    virtual RECT getButtonRect() const = 0;
    virtual void setGame(Game* game) = 0;
    virtual ~IRenderer() {}
};

class GDIRenderer : public IRenderer {
private:
    Game* m_game;
    RECT m_btnRect;

    void fillRoundRect(Graphics& gfx, SolidBrush* brush, int x, int y, int w, int h, int r);
    void drawText(Graphics& gfx, const wchar_t* text, Font* font, RectF rect, SolidBrush* brush);
    void drawText(Graphics& gfx, const wchar_t* text, Font* font, RectF rect, SolidBrush* brush, StringAlignment hAlign);
    void drawTile(Graphics& gfx, int x, int y, int value);
    void drawTile(Graphics& gfx, int x, int y, const Tile& tile);
    void drawScoreBox(Graphics& gfx, FontFamily& ff, int x, int y, int w, int h, const wchar_t* label, int score);

public:
    GDIRenderer();
    ~GDIRenderer() override;

    void renderFrame(HDC hdc, HWND hwnd) override;
    RECT getButtonRect() const override;
    void setGame(Game* game) override;
};
