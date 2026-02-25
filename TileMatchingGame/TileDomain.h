#pragma once

namespace TileMatchingGame {

enum class TileType {
    Bamboo,
    Circle,
    Flag,
    Bag,
    Earring,
    Joker,
};

enum class Participant {
    Player,
    AI,
};

enum class PlacementResult {
    Success,
    OutOfBounds,
    Occupied,
    InvalidTile,
    NoSpawnTile,
};

struct Tile {
    TileType type;
};

bool IsRegularTile(TileType type);
const wchar_t* TileTypeToText(TileType type);

}  // namespace TileMatchingGame
