/*
 * gdi_renderer.cpp
 * Implements GDIRenderer — all GDI+ drawing operations for the game board,
 * tiles, score boxes, buttons, and win/game-over overlays.
 */

#include "renderer.h"
#include "game.h"

// Returns the background color, text color, and font size for a given tile value
TileTheme getTileTheme(int value) {
    switch (value) {
        case 0:    return { 0xFFCDC1B4, 0x00000000, 0 };
        case 2:    return { 0xFFEEE4DA, 0xFF776E65, 48 };
        case 4:    return { 0xFFEDE0C8, 0xFF776E65, 48 };
        case 8:    return { 0xFFF2B179, 0xFFF9F6F2, 48 };
        case 16:   return { 0xFFF59563, 0xFFF9F6F2, 48 };
        case 32:   return { 0xFFF67C5F, 0xFFF9F6F2, 48 };
        case 64:   return { 0xFFF65E3B, 0xFFF9F6F2, 48 };
        case 128:  return { 0xFFEDCF72, 0xFFF9F6F2, 40 };
        case 256:  return { 0xFFEDCC61, 0xFFF9F6F2, 40 };
        case 512:  return { 0xFFEDC850, 0xFFF9F6F2, 40 };
        case 1024: return { 0xFFEDC53F, 0xFFF9F6F2, 32 };
        case 2048: return { 0xFFEDC22E, 0xFFF9F6F2, 32 };
        default:   return { 0xFF3C3A32, 0xFFF9F6F2, 28 };
    }
}

// Converts an integer to a wide string using Win32 API
static void intToWstr(int val, wchar_t* buf) {
    wsprintfW(buf, L"%d", val);
}

GDIRenderer::GDIRenderer() : m_game(nullptr), m_btnRect{0, 0, 0, 0} {}

GDIRenderer::~GDIRenderer() {}

// Sets the game instance this renderer will draw
void GDIRenderer::setGame(Game* game) { m_game = game; }

// Returns the bounding rectangle of the "New Game" button for hit testing
RECT GDIRenderer::getButtonRect() const { return m_btnRect; }

// Draws a filled rectangle with rounded corners
void GDIRenderer::fillRoundRect(Graphics& gfx, SolidBrush* brush,
                                 int x, int y, int w, int h, int r) {
    GraphicsPath path;
    path.AddArc(x, y, r * 2, r * 2, 180, 90);
    path.AddArc(x + w - r * 2, y, r * 2, r * 2, 270, 90);
    path.AddArc(x + w - r * 2, y + h - r * 2, r * 2, r * 2, 0, 90);
    path.AddArc(x, y + h - r * 2, r * 2, r * 2, 90, 90);
    path.CloseFigure();
    gfx.FillPath(brush, &path);
}

// Draws centered text within a rectangle
void GDIRenderer::drawText(Graphics& gfx, const wchar_t* text, Font* font,
                            RectF rect, SolidBrush* brush) {
    StringFormat sf;
    sf.SetAlignment(StringAlignmentCenter);
    sf.SetLineAlignment(StringAlignmentCenter);
    gfx.DrawString(text, -1, font, rect, &sf, brush);
}

// Draws text within a rectangle with custom horizontal alignment
void GDIRenderer::drawText(Graphics& gfx, const wchar_t* text, Font* font,
                            RectF rect, SolidBrush* brush, StringAlignment hAlign) {
    StringFormat sf;
    sf.SetAlignment(hAlign);
    sf.SetLineAlignment(StringAlignmentCenter);
    gfx.DrawString(text, -1, font, rect, &sf, brush);
}

// Draws a tile at the given position using an integer value
void GDIRenderer::drawTile(Graphics& gfx, int x, int y, int value) {
    TileTheme th = getTileTheme(value);
    SolidBrush brTile{Color{th.bg}};
    fillRoundRect(gfx, &brTile, x, y, TILE_SIZE, TILE_SIZE, 6);

    if (value > 0) {
        FontFamily ff(L"Arial");
        Font fTile(&ff, th.fontSize, FontStyleBold, UnitPixel);
        SolidBrush brText{Color{th.fg}};
        wchar_t buf[32];
        intToWstr(value, buf);
        RectF rTile{(REAL)x, (REAL)y, (REAL)TILE_SIZE, (REAL)TILE_SIZE};
        drawText(gfx, buf, &fTile, rTile, &brText);
    }
}

// Draws a tile at the given position using a Tile object reference
void GDIRenderer::drawTile(Graphics& gfx, int x, int y, const Tile& tile) {
    drawTile(gfx, x, y, tile.getValue());
}

// Draws a labeled score box (e.g., "SCORE" or "BEST") with the given value
void GDIRenderer::drawScoreBox(Graphics& gfx, FontFamily& ff,
                                int x, int y, int w, int h,
                                const wchar_t* label, int score) {
    SolidBrush brBg{Color{CLR_SCORE_BG}};
    fillRoundRect(gfx, &brBg, x, y, w, h, 6);

    Font fLabel(&ff, 11, FontStyleBold, UnitPixel);
    SolidBrush brLabel{Color{CLR_LABEL}};
    RectF rLabel{(REAL)x, (REAL)(y + 6), (REAL)w, 16};
    drawText(gfx, label, &fLabel, rLabel, &brLabel);

    Font fVal(&ff, 24, FontStyleBold, UnitPixel);
    SolidBrush brVal{Color{CLR_LIGHT}};
    wchar_t buf[32];
    intToWstr(score, buf);
    RectF rVal{(REAL)x, (REAL)(y + 22), (REAL)w, 28};
    drawText(gfx, buf, &fVal, rVal, &brVal);
}

// Renders the entire game frame: background, header, board, tiles, and overlays
void GDIRenderer::renderFrame(HDC hdc, HWND hwnd) {
    if (!m_game) return;

    RECT rc;
    GetClientRect(hwnd, &rc);
    int cw = rc.right, ch = rc.bottom;

    HDC mem = CreateCompatibleDC(hdc);
    HBITMAP bmp = CreateCompatibleBitmap(hdc, cw, ch);
    HBITMAP oldBmp = (HBITMAP)SelectObject(mem, bmp);

    Graphics gfx(mem);
    gfx.SetSmoothingMode(SmoothingModeAntiAlias);
    gfx.SetTextRenderingHint(TextRenderingHintAntiAlias);

    SolidBrush brBg{Color{CLR_BG}};
    gfx.FillRectangle(&brBg, 0, 0, cw, ch);

    int bx = (cw - BOARD_RENDER_SIZE) / 2;
    int by = BOARD_Y_OFFSET;
    FontFamily ff(L"Arial");

    Font fTitle(&ff, 64, FontStyleBold, UnitPixel);
    SolidBrush brTitle{Color{CLR_DARK}};
    RectF rTitle{(REAL)bx, 10, 200, 80};
    drawText(gfx, L"2048", &fTitle, rTitle, &brTitle, StringAlignmentNear);

    Font fSub(&ff, 13, FontStyleRegular, UnitPixel);
    SolidBrush brSub{Color{CLR_SUBTITLE}};
    RectF rSub{(REAL)bx, 82, 300, 20};
    drawText(gfx, L"Join the tiles, get to 2048!", &fSub, rSub, &brSub, StringAlignmentNear);

    int sbW = 95, sbH = 55, sbY = 15;
    int bestX = bx + BOARD_RENDER_SIZE - sbW;
    int scoreX = bestX - sbW - 8;

    drawScoreBox(gfx, ff, scoreX, sbY, sbW, sbH, L"SCORE", m_game->getScore());
    drawScoreBox(gfx, ff, bestX, sbY, sbW, sbH, L"BEST", ScoreManager::getBestScore());

    int btnW = 120, btnH = 38;
    int btnX = bx + BOARD_RENDER_SIZE - btnW, btnY = 88;
    SolidBrush brBtn{Color{CLR_BTN}};
    fillRoundRect(gfx, &brBtn, btnX, btnY, btnW, btnH, 5);
    Font fBtn(&ff, 15, FontStyleBold, UnitPixel);
    SolidBrush brBtnTxt{Color{CLR_LIGHT}};
    RectF rBtn{(REAL)btnX, (REAL)btnY, (REAL)btnW, (REAL)btnH};
    drawText(gfx, L"New Game", &fBtn, rBtn, &brBtnTxt);
    m_btnRect = { btnX, btnY, btnX + btnW, btnY + btnH };

    SolidBrush brBoard{Color{CLR_BOARD_BG}};
    fillRoundRect(gfx, &brBoard, bx, by, BOARD_RENDER_SIZE, BOARD_RENDER_SIZE, 8);

    for (int i = 0; i < GRID_DIM; i++) {
        for (int j = 0; j < GRID_DIM; j++) {
            int tx = bx + BOARD_PADDING + j * (TILE_SIZE + TILE_GAP);
            int ty = by + BOARD_PADDING + i * (TILE_SIZE + TILE_GAP);
            drawTile(gfx, tx, ty, m_game->getTile(i, j));
        }
    }

    if ((m_game->hasWon() && !m_game->isKeepPlaying()) || m_game->isOver()) {
        SolidBrush brOverlay{Color{CLR_OVERLAY}};
        fillRoundRect(gfx, &brOverlay, bx, by, BOARD_RENDER_SIZE, BOARD_RENDER_SIZE, 8);

        const wchar_t* msg = m_game->isOver() ? L"Game Over!" : L"You Win!";
        ARGB msgClr = m_game->isOver() ? CLR_DARK : 0xFFF65E3B;
        Font fMsg(&ff, 56, FontStyleBold, UnitPixel);
        SolidBrush brMsg{Color{msgClr}};
        RectF rMsg{(REAL)bx, (REAL)by, (REAL)BOARD_RENDER_SIZE, (REAL)(BOARD_RENDER_SIZE - 40)};
        drawText(gfx, msg, &fMsg, rMsg, &brMsg);

        const wchar_t* sub = m_game->isOver()
            ? L"Click 'New Game' to try again"
            : L"Press 'C' to continue playing";
        Font fSub2(&ff, 18, FontStyleRegular, UnitPixel);
        SolidBrush brSub2{Color{CLR_DARK}};
        RectF rSub2{(REAL)bx, (REAL)(by + 40), (REAL)BOARD_RENDER_SIZE, (REAL)(BOARD_RENDER_SIZE - 40)};
        drawText(gfx, sub, &fSub2, rSub2, &brSub2);
    }

    BitBlt(hdc, 0, 0, cw, ch, mem, 0, 0, SRCCOPY);
    SelectObject(mem, oldBmp);
    DeleteObject(bmp);
    DeleteDC(mem);
}
