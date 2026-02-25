#include <windows.h>

#include "BoardState.h"
#include "GameCore.h"
#include "TileDomain.h"

#include <sstream>
#include <string>

namespace {
constexpr int kClientWidth = 360;
constexpr int kClientHeight = 640;
constexpr wchar_t kWindowClassName[] = L"TileMatchingGameWindowClass";
constexpr wchar_t kWindowTitle[] = L"Tile Matching Game";

using namespace TileMatchingGame;

std::wstring BuildDebugStateText(const GameCore& game) {
    std::wostringstream oss;
    oss << L"[GameCore] playerTarget=" << TileTypeToText(game.GetPlayerTargetType())
        << L", aiTarget=" << TileTypeToText(game.GetAITargetType());

    const auto spawnTile = game.GetCurrentSpawnTile();
    if (spawnTile.has_value()) {
        oss << L", spawnTile=" << TileTypeToText(spawnTile->type);
    } else {
        oss << L", spawnTile=(none)";
    }

    if (game.GetWinner().has_value()) {
        oss << L", winner=" << (game.GetWinner().value() == Participant::Player ? L"Player" : L"AI");
    }

    oss << L"\n";
    return oss.str();
}

void RunCoreLogicSmokeTest() {
    GameCore game;

    for (int turn = 0; turn < 30 && !game.GetWinner().has_value(); ++turn) {
        game.SpawnTurnTile();

        if (game.IsWaitingForPlayer()) {
            bool placed = false;
            for (int row = 0; row < BoardState::kBoardSize && !placed; ++row) {
                for (int col = 0; col < BoardState::kBoardSize && !placed; ++col) {
                    const auto result = game.TryPlacePlayerTile(row, col);
                    if (result == PlacementResult::Success || result == PlacementResult::Occupied) {
                        placed = (result == PlacementResult::Success);
                    }
                }
            }
        }
    }

    const std::wstring debugText = BuildDebugStateText(game);
    OutputDebugStringW(debugText.c_str());
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}
}  // namespace

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow) {
    RunCoreLogicSmokeTest();

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = kWindowClassName;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);

    if (!RegisterClassExW(&wc)) {
        return 0;
    }

    RECT desiredRect{0, 0, kClientWidth, kClientHeight};
    AdjustWindowRectEx(&desiredRect, WS_OVERLAPPEDWINDOW, FALSE, 0);

    const int windowWidth = desiredRect.right - desiredRect.left;
    const int windowHeight = desiredRect.bottom - desiredRect.top;

    HWND hwnd = CreateWindowExW(
        0,
        kWindowClassName,
        kWindowTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        windowWidth,
        windowHeight,
        nullptr,
        nullptr,
        hInstance,
        nullptr);

    if (!hwnd) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg{};
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return static_cast<int>(msg.wParam);
}
