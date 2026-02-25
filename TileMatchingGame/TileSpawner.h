#pragma once

#include "TileDomain.h"

#include <array>
#include <random>

namespace TileMatchingGame {

class TileSpawner {
public:
    TileSpawner();
    Tile Spawn();

private:
    std::mt19937 random_;
    std::uniform_int_distribution<int> rollType_;
    std::uniform_int_distribution<int> rollRegularType_;
    const std::array<TileType, 5> regularTypes_;
};

}  // namespace TileMatchingGame
