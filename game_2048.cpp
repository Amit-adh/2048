/**
 * 2048 GUI Game in C++ (Win32 API + GDI+)
 *
 * Controls:
 *   Arrow Keys / WASD  - Move tiles
 *   Click "New Game"   - Restart
 *   C                  - Continue after winning
 *
 * Compile:
 *   g++ -o game_2048.exe game_2048.cpp -std=c++17 -lgdi32 -lgdiplus -mwindows
 */

#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>
#include <gdiplus.h>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cstdio>
#include <vector>

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "gdi32.lib")

using namespace Gdiplus;

// ─── Helpers ────────────────────────────────────────────────────────
static void intToWstr(int val, wchar_t* buf, int bufLen) {
    wsprintfW(buf, L"%d", val);
}

// ─── Constants ──────────────────────────────────────────────────────
static const int GRID = 4;
static const int WIN_VALUE = 2048;

static const int TILE_SIZE = 107;
static const int TILE_GAP = 12;
static const int BOARD_PADDING = 12;
static const int BOARD_Y_OFFSET = 140;

static const int BOARD_SIZE = BOARD_PADDING * 2 + GRID * TILE_SIZE + (GRID - 1) * TILE_GAP;
static const int WINDOW_W = BOARD_SIZE + 40;
static const int WINDOW_H = BOARD_Y_OFFSET + BOARD_SIZE + 40;

// ─── Tile color palette (original 2048 colors) ──────────────────────
struct TileTheme {
    ARGB bg;
    ARGB fg;
    float fontSize;
};

static TileTheme getTileTheme(int value) {
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

static const ARGB CLR_BG       = 0xFFFAF8EF;
static const ARGB CLR_BOARD_BG = 0xFFBBADA0;
static const ARGB CLR_DARK     = 0xFF776E65;
static const ARGB CLR_LIGHT    = 0xFFF9F6F2;
static const ARGB CLR_SCORE_BG = 0xFFBBADA0;
static const ARGB CLR_BTN      = 0xFF8F7A66;
static const ARGB CLR_OVERLAY  = 0xBBFAF8EF;
static const ARGB CLR_LABEL    = 0xFFEEE4DA;
static const ARGB CLR_SUBTITLE = 0xFFA0968B;

// ─── Game State ─────────────────────────────────────────────────────
struct Game {
    int board[GRID][GRID];
    int score;
    int bestScore;
    bool won;
    bool keepPlaying;
    bool gameOver;
};

static Game g_game;
static RECT g_newGameBtn = { 0, 0, 0, 0 };

// ─── Board Operations ───────────────────────────────────────────────
static void initBoard() {
    for (int i = 0; i < GRID; i++)
        for (int j = 0; j < GRID; j++)
            g_game.board[i][j] = 0;
    g_game.score = 0;
    g_game.won = false;
    g_game.keepPlaying = false;
    g_game.gameOver = false;
}

static bool spawnTile() {
    std::vector<std::pair<int,int>> empty;
    for (int i = 0; i < GRID; i++)
        for (int j = 0; j < GRID; j++)
            if (g_game.board[i][j] == 0)
                empty.push_back({i, j});
    if (empty.empty()) return false;
    int idx = rand() % empty.size();
    int val = (rand() % 10 < 9) ? 2 : 4;
    g_game.board[empty[idx].first][empty[idx].second] = val;
    return true;
}

// ─── Movement Logic ─────────────────────────────────────────────────
static bool moveLeft() {
    bool moved = false;
    for (int i = 0; i < GRID; i++) {
        int merged[GRID] = {};
        for (int j = 1; j < GRID; j++) {
            if (g_game.board[i][j] == 0) continue;
            int k = j;
            while (k > 0 && g_game.board[i][k-1] == 0) {
                g_game.board[i][k-1] = g_game.board[i][k];
                g_game.board[i][k] = 0;
                k--; moved = true;
            }
            if (k > 0 && g_game.board[i][k-1] == g_game.board[i][k] && !merged[k-1]) {
                g_game.board[i][k-1] *= 2;
                g_game.score += g_game.board[i][k-1];
                g_game.board[i][k] = 0;
                merged[k-1] = 1; moved = true;
                if (g_game.board[i][k-1] == WIN_VALUE) g_game.won = true;
            }
        }
    }
    return moved;
}

static bool moveRight() {
    bool moved = false;
    for (int i = 0; i < GRID; i++) {
        int merged[GRID] = {};
        for (int j = GRID - 2; j >= 0; j--) {
            if (g_game.board[i][j] == 0) continue;
            int k = j;
            while (k < GRID - 1 && g_game.board[i][k+1] == 0) {
                g_game.board[i][k+1] = g_game.board[i][k];
                g_game.board[i][k] = 0;
                k++; moved = true;
            }
            if (k < GRID - 1 && g_game.board[i][k+1] == g_game.board[i][k] && !merged[k+1]) {
                g_game.board[i][k+1] *= 2;
                g_game.score += g_game.board[i][k+1];
                g_game.board[i][k] = 0;
                merged[k+1] = 1; moved = true;
                if (g_game.board[i][k+1] == WIN_VALUE) g_game.won = true;
            }
        }
    }
    return moved;
}

static bool moveUp() {
    bool moved = false;
    for (int j = 0; j < GRID; j++) {
        int merged[GRID] = {};
        for (int i = 1; i < GRID; i++) {
            if (g_game.board[i][j] == 0) continue;
            int k = i;
            while (k > 0 && g_game.board[k-1][j] == 0) {
                g_game.board[k-1][j] = g_game.board[k][j];
                g_game.board[k][j] = 0;
                k--; moved = true;
            }
            if (k > 0 && g_game.board[k-1][j] == g_game.board[k][j] && !merged[k-1]) {
                g_game.board[k-1][j] *= 2;
                g_game.score += g_game.board[k-1][j];
                g_game.board[k][j] = 0;
                merged[k-1] = 1; moved = true;
                if (g_game.board[k-1][j] == WIN_VALUE) g_game.won = true;
            }
        }
    }
    return moved;
}

static bool moveDown() {
    bool moved = false;
    for (int j = 0; j < GRID; j++) {
        int merged[GRID] = {};
        for (int i = GRID - 2; i >= 0; i--) {
            if (g_game.board[i][j] == 0) continue;
            int k = i;
            while (k < GRID - 1 && g_game.board[k+1][j] == 0) {
                g_game.board[k+1][j] = g_game.board[k][j];
                g_game.board[k][j] = 0;
                k++; moved = true;
            }
            if (k < GRID - 1 && g_game.board[k+1][j] == g_game.board[k][j] && !merged[k+1]) {
                g_game.board[k+1][j] *= 2;
                g_game.score += g_game.board[k+1][j];
                g_game.board[k][j] = 0;
                merged[k+1] = 1; moved = true;
                if (g_game.board[k+1][j] == WIN_VALUE) g_game.won = true;
            }
        }
    }
    return moved;
}

static bool canMove() {
    for (int i = 0; i < GRID; i++)
        for (int j = 0; j < GRID; j++) {
            if (g_game.board[i][j] == 0) return true;
            if (j < GRID - 1 && g_game.board[i][j] == g_game.board[i][j+1]) return true;
            if (i < GRID - 1 && g_game.board[i][j] == g_game.board[i+1][j]) return true;
        }
    return false;
}

// ─── GDI+ Drawing Helpers ───────────────────────────────────────────
static void fillRoundRect(Graphics &gfx, SolidBrush *brush, int x, int y, int w, int h, int r) {
    GraphicsPath path;
    path.AddArc(x, y, r * 2, r * 2, 180, 90);
    path.AddArc(x + w - r * 2, y, r * 2, r * 2, 270, 90);
    path.AddArc(x + w - r * 2, y + h - r * 2, r * 2, r * 2, 0, 90);
    path.AddArc(x, y + h - r * 2, r * 2, r * 2, 90, 90);
    path.CloseFigure();
    gfx.FillPath(brush, &path);
}

static void drawCentered(Graphics &gfx, const wchar_t* text, Font *font, RectF rect, SolidBrush *brush) {
    StringFormat sf;
    sf.SetAlignment(StringAlignmentCenter);
    sf.SetLineAlignment(StringAlignmentCenter);
    gfx.DrawString(text, -1, font, rect, &sf, brush);
}

// ─── Rendering ──────────────────────────────────────────────────────
static void PaintGame(HWND hwnd, HDC hdc) {
    RECT rc;
    GetClientRect(hwnd, &rc);
    int cw = rc.right, ch = rc.bottom;

    // Double buffer
    HDC mem = CreateCompatibleDC(hdc);
    HBITMAP bmp = CreateCompatibleBitmap(hdc, cw, ch);
    HBITMAP oldBmp = (HBITMAP)SelectObject(mem, bmp);

    Graphics gfx(mem);
    gfx.SetSmoothingMode(SmoothingModeAntiAlias);
    gfx.SetTextRenderingHint(TextRenderingHintAntiAlias);

    // Background
    SolidBrush brBg{Color{CLR_BG}};
    gfx.FillRectangle(&brBg, 0, 0, cw, ch);

    int bx = (cw - BOARD_SIZE) / 2;
    int by = BOARD_Y_OFFSET;

    FontFamily ff(L"Arial");
    wchar_t numBuf[32];

    // ── Title ───────────────────────────────────────────
    Font fTitle(&ff, 64, FontStyleBold, UnitPixel);
    SolidBrush brTitle{Color{CLR_DARK}};
    RectF rTitle{(REAL)bx, 10, 200, 80};
    StringFormat sfL;
    sfL.SetAlignment(StringAlignmentNear);
    sfL.SetLineAlignment(StringAlignmentCenter);
    gfx.DrawString(L"2048", -1, &fTitle, rTitle, &sfL, &brTitle);

    // ── Subtitle ────────────────────────────────────────
    Font fSub(&ff, 13, FontStyleRegular, UnitPixel);
    SolidBrush brSub{Color{CLR_SUBTITLE}};
    RectF rSub{(REAL)bx, 82, 300, 20};
    gfx.DrawString(L"Join the tiles, get to 2048!", -1, &fSub, rSub, &sfL, &brSub);

    // ── Score boxes ─────────────────────────────────────
    int sbW = 95, sbH = 55, sbY = 15;
    int bestX = bx + BOARD_SIZE - sbW;
    int scoreX = bestX - sbW - 8;

    SolidBrush brScoreBg{Color{CLR_SCORE_BG}};
    Font fScoreLabel(&ff, 11, FontStyleBold, UnitPixel);
    Font fScoreVal(&ff, 24, FontStyleBold, UnitPixel);
    SolidBrush brLabel{Color{CLR_LABEL}};
    SolidBrush brValText{Color{CLR_LIGHT}};

    // SCORE
    fillRoundRect(gfx, &brScoreBg, scoreX, sbY, sbW, sbH, 6);
    RectF rScLabel{(REAL)scoreX, (REAL)(sbY + 6), (REAL)sbW, 16};
    drawCentered(gfx, L"SCORE", &fScoreLabel, rScLabel, &brLabel);
    intToWstr(g_game.score, numBuf, 32);
    RectF rScVal{(REAL)scoreX, (REAL)(sbY + 22), (REAL)sbW, 28};
    drawCentered(gfx, numBuf, &fScoreVal, rScVal, &brValText);

    // BEST
    fillRoundRect(gfx, &brScoreBg, bestX, sbY, sbW, sbH, 6);
    RectF rBestLabel{(REAL)bestX, (REAL)(sbY + 6), (REAL)sbW, 16};
    drawCentered(gfx, L"BEST", &fScoreLabel, rBestLabel, &brLabel);
    intToWstr(g_game.bestScore, numBuf, 32);
    RectF rBestVal{(REAL)bestX, (REAL)(sbY + 22), (REAL)sbW, 28};
    drawCentered(gfx, numBuf, &fScoreVal, rBestVal, &brValText);

    // ── New Game button ─────────────────────────────────
    int btnW = 120, btnH = 38;
    int btnX = bx + BOARD_SIZE - btnW, btnY = 88;
    SolidBrush brBtn{Color{CLR_BTN}};
    fillRoundRect(gfx, &brBtn, btnX, btnY, btnW, btnH, 5);
    Font fBtn(&ff, 15, FontStyleBold, UnitPixel);
    SolidBrush brBtnTxt{Color{CLR_LIGHT}};
    RectF rBtn{(REAL)btnX, (REAL)btnY, (REAL)btnW, (REAL)btnH};
    drawCentered(gfx, L"New Game", &fBtn, rBtn, &brBtnTxt);
    g_newGameBtn = { btnX, btnY, btnX + btnW, btnY + btnH };

    // ── Board background ────────────────────────────────
    SolidBrush brBoard{Color{CLR_BOARD_BG}};
    fillRoundRect(gfx, &brBoard, bx, by, BOARD_SIZE, BOARD_SIZE, 8);

    // ── Tiles ───────────────────────────────────────────
    for (int i = 0; i < GRID; i++) {
        for (int j = 0; j < GRID; j++) {
            int tx = bx + BOARD_PADDING + j * (TILE_SIZE + TILE_GAP);
            int ty = by + BOARD_PADDING + i * (TILE_SIZE + TILE_GAP);
            int val = g_game.board[i][j];

            TileTheme th = getTileTheme(val);
            SolidBrush brTile{Color{th.bg}};
            fillRoundRect(gfx, &brTile, tx, ty, TILE_SIZE, TILE_SIZE, 6);

            if (val > 0) {
                Font fTile(&ff, th.fontSize, FontStyleBold, UnitPixel);
                SolidBrush brTileText{Color{th.fg}};
                intToWstr(val, numBuf, 32);
                RectF rTile{(REAL)tx, (REAL)ty, (REAL)TILE_SIZE, (REAL)TILE_SIZE};
                drawCentered(gfx, numBuf, &fTile, rTile, &brTileText);
            }
        }
    }

    // ── Win / Game Over Overlay ─────────────────────────
    if ((g_game.won && !g_game.keepPlaying) || g_game.gameOver) {
        SolidBrush brOverlay{Color{CLR_OVERLAY}};
        fillRoundRect(gfx, &brOverlay, bx, by, BOARD_SIZE, BOARD_SIZE, 8);

        const wchar_t* msg = g_game.gameOver ? L"Game Over!" : L"You Win!";
        ARGB msgClr = g_game.gameOver ? CLR_DARK : 0xFFF65E3B;
        Font fMsg(&ff, 56, FontStyleBold, UnitPixel);
        SolidBrush brMsg{Color{msgClr}};
        RectF rMsg{(REAL)bx, (REAL)by, (REAL)BOARD_SIZE, (REAL)(BOARD_SIZE - 40)};
        drawCentered(gfx, msg, &fMsg, rMsg, &brMsg);

        const wchar_t* sub = g_game.gameOver
            ? L"Click 'New Game' to try again"
            : L"Press 'C' to continue playing";
        Font fSub2(&ff, 18, FontStyleRegular, UnitPixel);
        SolidBrush brSub2{Color{CLR_DARK}};
        RectF rSub2{(REAL)bx, (REAL)(by + 40), (REAL)BOARD_SIZE, (REAL)(BOARD_SIZE - 40)};
        drawCentered(gfx, sub, &fSub2, rSub2, &brSub2);
    }

    // Blit
    BitBlt(hdc, 0, 0, cw, ch, mem, 0, 0, SRCCOPY);
    SelectObject(mem, oldBmp);
    DeleteObject(bmp);
    DeleteDC(mem);
}

// ─── Process Move ───────────────────────────────────────────────────
static void afterMove(HWND hwnd, bool moved) {
    if (moved) {
        spawnTile();
        if (g_game.score > g_game.bestScore)
            g_game.bestScore = g_game.score;
        if (!canMove())
            g_game.gameOver = true;
        InvalidateRect(hwnd, NULL, FALSE);
    }
}

static void newGame(HWND hwnd) {
    initBoard();
    spawnTile();
    spawnTile();
    InvalidateRect(hwnd, NULL, FALSE);
}

// ─── Window Procedure ───────────────────────────────────────────────
static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            PaintGame(hwnd, hdc);
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_ERASEBKGND:
            return 1;

        case WM_KEYDOWN: {
            if (g_game.won && !g_game.keepPlaying) {
                if (wParam == 'C') {
                    g_game.keepPlaying = true;
                    InvalidateRect(hwnd, NULL, FALSE);
                }
                return 0;
            }
            if (g_game.gameOver) return 0;

            bool moved = false;
            switch (wParam) {
                case VK_UP:    case 'W': moved = moveUp();    break;
                case VK_DOWN:  case 'S': moved = moveDown();  break;
                case VK_LEFT:  case 'A': moved = moveLeft();  break;
                case VK_RIGHT: case 'D': moved = moveRight(); break;
            }
            afterMove(hwnd, moved);
            return 0;
        }

        case WM_LBUTTONDOWN: {
            POINT pt = { LOWORD(lParam), HIWORD(lParam) };
            if (PtInRect(&g_newGameBtn, pt))
                newGame(hwnd);
            return 0;
        }

        case WM_GETMINMAXINFO: {
            MINMAXINFO* mmi = (MINMAXINFO*)lParam;
            mmi->ptMinTrackSize.x = WINDOW_W;
            mmi->ptMinTrackSize.y = WINDOW_H;
            return 0;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// ─── Entry Point ────────────────────────────────────────────────────
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nShow) {
    srand((unsigned)time(nullptr));

    // GDI+ init
    GdiplusStartupInput gsi;
    ULONG_PTR gdiToken;
    GdiplusStartup(&gdiToken, &gsi, NULL);

    // Window class
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = L"Game2048";
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    RegisterClassEx(&wc);

    // Size window for desired client area
    RECT wr = { 0, 0, WINDOW_W, WINDOW_H };
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME, FALSE);

    HWND hwnd = CreateWindowEx(
        0, L"Game2048", L"2048",
        (WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX) | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        wr.right - wr.left, wr.bottom - wr.top,
        NULL, NULL, hInst, NULL
    );
    if (!hwnd) return 1;

    initBoard();
    spawnTile();
    spawnTile();

    ShowWindow(hwnd, nShow);
    UpdateWindow(hwnd);

    MSG m;
    while (GetMessage(&m, NULL, 0, 0)) {
        TranslateMessage(&m);
        DispatchMessage(&m);
    }

    GdiplusShutdown(gdiToken);
    return (int)m.wParam;
}
