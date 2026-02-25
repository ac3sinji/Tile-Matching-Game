#include <windows.h>
#include <windowsx.h>

#include "BoardState.h"
#include "GameCore.h"
#include "TileDomain.h"

#include <optional>
#include <sstream>
#include <string>

namespace {
constexpr int kClientWidth = 360;
constexpr int kClientHeight = 640;
constexpr wchar_t kWindowClassName[] = L"TileMatchingGameWindowClass";
constexpr wchar_t kWindowTitle[] = L"Tile Matching Game";
constexpr UINT_PTR kTurnTickTimerId = 1;
constexpr UINT kTurnTickMs = 900;

using namespace TileMatchingGame;

struct RectInt {
    int left;
    int top;
    int right;
    int bottom;
};

struct AppState {
    GameCore game;
    std::wstring statusText = L"게임 시작";
    RectInt playerBoardRect{30, 170, 330, 470};
};

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

wchar_t TileTypeToGlyph(TileType type) {
    switch (type) {
    case TileType::Bamboo:
        return L'B';
    case TileType::Circle:
        return L'C';
    case TileType::Flag:
        return L'F';
    case TileType::Bag:
        return L'G';
    case TileType::Earring:
        return L'E';
    case TileType::Joker:
        return L'J';
    default:
        return L'?';
    }
}

void DrawBoard(HDC hdc, const BoardState& board, const RectInt& rect) {
    Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);

    const int size = BoardState::kBoardSize;
    const int cellW = (rect.right - rect.left) / size;
    const int cellH = (rect.bottom - rect.top) / size;

    for (int i = 1; i < size; ++i) {
        MoveToEx(hdc, rect.left + i * cellW, rect.top, nullptr);
        LineTo(hdc, rect.left + i * cellW, rect.bottom);

        MoveToEx(hdc, rect.left, rect.top + i * cellH, nullptr);
        LineTo(hdc, rect.right, rect.top + i * cellH);
    }

    for (int row = 0; row < size; ++row) {
        for (int col = 0; col < size; ++col) {
            const auto& cell = board.GetCell(row, col);
            if (!cell.has_value()) {
                continue;
            }

            const int centerX = rect.left + col * cellW + cellW / 2;
            const int centerY = rect.top + row * cellH + cellH / 2;

            wchar_t glyph[2] = {TileTypeToGlyph(cell->type), 0};
            TextOutW(hdc, centerX - 4, centerY - 8, glyph, 1);
        }
    }
}

std::wstring BuildStatusText(const AppState& state) {
    if (state.game.GetWinner().has_value()) {
        return state.game.GetWinner().value() == Participant::Player ? L"승리!" : L"패배!";
    }

    if (state.game.IsWaitingForPlayer()) {
        const auto tile = state.game.GetCurrentSpawnTile();
        if (tile.has_value()) {
            return std::wstring(L"클릭해서 배치: ") + TileTypeToText(tile->type);
        }
        return L"클릭해서 배치";
    }

    return L"턴 진행 중...";
}

bool TryMapPointToCell(const RectInt& rect, int x, int y, int& outRow, int& outCol) {
    if (x < rect.left || x >= rect.right || y < rect.top || y >= rect.bottom) {
        return false;
    }

    const int cellW = (rect.right - rect.left) / BoardState::kBoardSize;
    const int cellH = (rect.bottom - rect.top) / BoardState::kBoardSize;

    outCol = (x - rect.left) / cellW;
    outRow = (y - rect.top) / cellH;
    return true;
}

void PaintWindow(HWND hwnd, AppState& state) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    SetBkMode(hdc, TRANSPARENT);

    std::wstring targetText = std::wstring(L"플레이어 목표: ") + TileTypeToText(state.game.GetPlayerTargetType()) +
        L" / AI 목표: " + TileTypeToText(state.game.GetAITargetType());
    TextOutW(hdc, 20, 20, targetText.c_str(), static_cast<int>(targetText.size()));

    const auto spawn = state.game.GetCurrentSpawnTile();
    std::wstring spawnText = L"현재 스폰 타일: ";
    spawnText += spawn.has_value() ? TileTypeToText(spawn->type) : L"없음";
    TextOutW(hdc, 20, 50, spawnText.c_str(), static_cast<int>(spawnText.size()));

    state.statusText = BuildStatusText(state);
    TextOutW(hdc, 20, 80, state.statusText.c_str(), static_cast<int>(state.statusText.size()));

    std::wstring usage = L"보드 칸을 클릭해 타일 배치";
    TextOutW(hdc, 20, 110, usage.c_str(), static_cast<int>(usage.size()));

    std::wstring playerLabel = L"Player";
    TextOutW(hdc, state.playerBoardRect.left, state.playerBoardRect.top - 22, playerLabel.c_str(), static_cast<int>(playerLabel.size()));
    DrawBoard(hdc, state.game.GetPlayerBoard(), state.playerBoardRect);

    RectInt aiRect{state.playerBoardRect.left, state.playerBoardRect.bottom + 55, state.playerBoardRect.right, state.playerBoardRect.bottom + 150};
    std::wstring aiLabel = L"AI 채운 칸 수";
    TextOutW(hdc, aiRect.left, aiRect.top - 22, aiLabel.c_str(), static_cast<int>(aiLabel.size()));

    const BoardState& aiBoard = state.game.GetAIBoard();
    int aiFilled = 0;
    for (int row = 0; row < BoardState::kBoardSize; ++row) {
        for (int col = 0; col < BoardState::kBoardSize; ++col) {
            if (aiBoard.GetCell(row, col).has_value()) {
                ++aiFilled;
            }
        }
    }

    std::wstring aiCounter = std::to_wstring(aiFilled) + L" / 9";
    TextOutW(hdc, aiRect.left, aiRect.top, aiCounter.c_str(), static_cast<int>(aiCounter.size()));

    EndPaint(hwnd, &ps);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    AppState* state = reinterpret_cast<AppState*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));

    switch (msg) {
    case WM_CREATE: {
        auto* createdState = new AppState{};
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(createdState));
        SetTimer(hwnd, kTurnTickTimerId, kTurnTickMs, nullptr);
        return 0;
    }
    case WM_TIMER:
        if (state && wParam == kTurnTickTimerId && !state->game.GetWinner().has_value()) {
            state->game.SpawnTurnTile();
            const std::wstring debugText = BuildDebugStateText(state->game);
            OutputDebugStringW(debugText.c_str());
            InvalidateRect(hwnd, nullptr, TRUE);
        }
        return 0;
    case WM_LBUTTONDOWN:
        if (state && state->game.IsWaitingForPlayer() && !state->game.GetWinner().has_value()) {
            const int x = GET_X_LPARAM(lParam);
            const int y = GET_Y_LPARAM(lParam);

            int row = -1;
            int col = -1;
            if (TryMapPointToCell(state->playerBoardRect, x, y, row, col)) {
                state->game.TryPlacePlayerTile(row, col);
                InvalidateRect(hwnd, nullptr, TRUE);
            }
        }
        return 0;
    case WM_PAINT:
        if (state) {
            PaintWindow(hwnd, *state);
            return 0;
        }
        break;
    case WM_DESTROY:
        if (state) {
            delete state;
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
        }
        KillTimer(hwnd, kTurnTickTimerId);
        PostQuitMessage(0);
        return 0;
    default:
        break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}
}  // namespace

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow) {
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
