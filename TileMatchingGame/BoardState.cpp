#include "BoardState.h"

namespace TileMatchingGame {

bool BoardState::IsFull() const {
    for (const auto& cell : cells_) {
        if (!cell.has_value()) {
            return false;
        }
    }
    return true;
}

int BoardState::FirstEmptyIndex() const {
    for (int index = 0; index < static_cast<int>(cells_.size()); ++index) {
        if (!cells_[index].has_value()) {
            return index;
        }
    }
    return -1;
}

PlacementResult BoardState::PlaceTile(int row, int col, const Tile& tile, TileType targetType) {
    if (row < 0 || row >= kBoardSize || col < 0 || col >= kBoardSize) {
        return PlacementResult::OutOfBounds;
    }

    const int index = row * kBoardSize + col;
    if (cells_[index].has_value()) {
        return PlacementResult::Occupied;
    }

    if (!(tile.type == targetType || tile.type == TileType::Joker)) {
        return PlacementResult::InvalidTile;
    }

    cells_[index] = tile;
    return PlacementResult::Success;
}

bool BoardState::PlaceFirstEmpty(const Tile& tile, TileType targetType) {
    const int index = FirstEmptyIndex();
    if (index < 0) {
        return false;
    }

    const int row = index / kBoardSize;
    const int col = index % kBoardSize;
    return PlaceTile(row, col, tile, targetType) == PlacementResult::Success;
}

}  // namespace TileMatchingGame
