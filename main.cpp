/*
 * main.cpp
 * Application entry point for the 2048 GUI game.
 * Creates the Win32 window, initializes GDI+, and runs the message loop.
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

#include "game.h"
#include "renderer.h"
#include "input_handler.h"

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "gdi32.lib")

using namespace Gdiplus;

static Game g_game;
static GDIRenderer g_renderer;

// Window procedure: handles paint, keyboard, mouse, and window messages
static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            g_renderer.renderFrame(hdc, hwnd);
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_ERASEBKGND:
            return 1;

        case WM_KEYDOWN:
            InputHandler::handleKeyDown(g_game, wParam, hwnd);
            return 0;

        case WM_LBUTTONDOWN: {
            int mx = LOWORD(lParam);
            int my = HIWORD(lParam);
            InputHandler::handleMouseClick(g_game, mx, my, hwnd);
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

// Entry point: initializes GDI+, creates window, starts the game
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nShow) {
    srand((unsigned)time(nullptr));

    GdiplusStartupInput gsi;
    ULONG_PTR gdiToken;
    GdiplusStartup(&gdiToken, &gsi, NULL);

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

    g_renderer.setGame(&g_game);
    g_game.setRenderer(&g_renderer);
    g_game.startNewGame();

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
