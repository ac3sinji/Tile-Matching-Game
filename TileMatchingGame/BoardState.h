#pragma once

#include "TileDomain.h"

#include <array>
#include <optional>

namespace TileMatchingGame {

class BoardState {
public:
    static constexpr int kBoardSize = 3;
    static constexpr int kBoardCellCount = kBoardSize * kBoardSize;

    bool IsFull() const;
    int FirstEmptyIndex() const;
    PlacementResult PlaceTile(int row, int col, const Tile& tile, TileType targetType);
    bool PlaceFirstEmpty(const Tile& tile, TileType targetType);

private:
    std::array<std::optional<Tile>, kBoardCellCount> cells_{};
};

}  // namespace TileMatchingGame
