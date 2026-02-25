#include <algorithm>
#include <iostream>
#include <random>
#include <string>
#include <utility>
#include <vector>

struct Position {
    int row;
    int col;
};

class TileMatchingGame {
public:
    TileMatchingGame(int rows, int cols)
        : rows_(rows), cols_(cols), board_(rows, std::vector<char>(cols, '.')) {
        InitializeBoard();
    }

    void Run() {
        std::cout << "=== Tile Matching Game (Console) ===\n";
        std::cout << "같은 타일 2개를 선택하면 제거됩니다.\n";
        std::cout << "입력 형식: 행 열 (예: 1 2)\n\n";

        while (!IsCleared()) {
            PrintBoard();
            Position first = PromptPosition("첫 번째 타일 선택");
            Position second = PromptPosition("두 번째 타일 선택");

            if (!IsValidPick(first) || !IsValidPick(second)) {
                std::cout << "잘못된 좌표입니다. 다시 시도하세요.\n\n";
                continue;
            }

            if (first.row == second.row && first.col == second.col) {
                std::cout << "같은 칸을 두 번 선택했습니다.\n\n";
                continue;
            }

            if (board_[first.row][first.col] == board_[second.row][second.col]) {
                std::cout << "매칭 성공!\n\n";
                board_[first.row][first.col] = '.';
                board_[second.row][second.col] = '.';
            }
            else {
                std::cout << "매칭 실패!\n\n";
            }
        }

        PrintBoard();
        std::cout << "모든 타일을 제거했습니다. 게임 클리어!\n";
    }

private:
    int rows_;
    int cols_;
    std::vector<std::vector<char>> board_;

    void InitializeBoard() {
        std::vector<char> tiles;
        const int pairCount = (rows_ * cols_) / 2;

        for (int i = 0; i < pairCount; ++i) {
            char symbol = static_cast<char>('A' + (i % 26));
            tiles.push_back(symbol);
            tiles.push_back(symbol);
        }

        std::random_device rd;
        std::mt19937 gen(rd());
        std::shuffle(tiles.begin(), tiles.end(), gen);

        int index = 0;
        for (int r = 0; r < rows_; ++r) {
            for (int c = 0; c < cols_; ++c) {
                board_[r][c] = tiles[index++];
            }
        }
    }

    void PrintBoard() const {
        std::cout << "   ";
        for (int c = 0; c < cols_; ++c) {
            std::cout << c << " ";
        }
        std::cout << "\n";

        for (int r = 0; r < rows_; ++r) {
            std::cout << r << "| ";
            for (int c = 0; c < cols_; ++c) {
                std::cout << board_[r][c] << " ";
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }

    Position PromptPosition(const std::string& message) const {
        Position pos{};
        std::cout << message << " (행 열): ";
        std::cin >> pos.row >> pos.col;
        return pos;
    }

    bool IsValidPick(const Position& pos) const {
        return pos.row >= 0 && pos.row < rows_ &&
               pos.col >= 0 && pos.col < cols_ &&
               board_[pos.row][pos.col] != '.';
    }

    bool IsCleared() const {
        for (const auto& row : board_) {
            for (char cell : row) {
                if (cell != '.') {
                    return false;
                }
            }
        }
        return true;
    }
};

int main() {
    TileMatchingGame game(4, 4);
    game.Run();
    return 0;
}
