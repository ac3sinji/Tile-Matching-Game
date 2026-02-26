#include <windows.h>

#include "MapSettingPanel.h"

namespace {
constexpr int kWindowWidth = 1000;
constexpr int kWindowHeight = 720;

void DestroyMapSettingPanel(HWND hwnd) {
    auto* panel = reinterpret_cast<MapSettingPanel*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    if (panel != nullptr) {
        delete panel;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, 0);
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE: {
        auto* panel = new MapSettingPanel();
        panel->Create(hwnd);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(panel));
        return 0;
    }
    case WM_COMMAND: {
        auto* panel = reinterpret_cast<MapSettingPanel*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        if (panel != nullptr && panel->HandleCommand(wParam)) {
            return 0;
        }
        break;
    }
    case WM_ERASEBKGND:
        return 1;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        FillRect(hdc, &ps.rcPaint, static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_DESTROY:
        DestroyMapSettingPanel(hwnd);
        PostQuitMessage(0);
        return 0;
    default:
        break;
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}
} // namespace

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow) {
    const wchar_t kClassName[] = L"TileMatchingGameWindow";

    WNDCLASS wc{};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    wc.lpszClassName = kClassName;

    if (!RegisterClass(&wc)) {
        return 1;
    }

    RECT desiredRect{0, 0, kWindowWidth, kWindowHeight};
    AdjustWindowRect(&desiredRect, WS_OVERLAPPEDWINDOW, FALSE);

    HWND hwnd = CreateWindowEx(
        0,
        kClassName,
        L"Tile Matching Game",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        desiredRect.right - desiredRect.left,
        desiredRect.bottom - desiredRect.top,
        nullptr,
        nullptr,
        hInstance,
        nullptr);

    if (!hwnd) {
        return 1;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg{};
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return static_cast<int>(msg.wParam);
}
