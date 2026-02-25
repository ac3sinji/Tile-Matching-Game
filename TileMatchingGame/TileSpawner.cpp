#include "TileSpawner.h"

namespace TileMatchingGame {

TileSpawner::TileSpawner()
    : random_(std::random_device{}()),
      rollType_(0, 99),
      rollRegularType_(0, static_cast<int>(regularTypes_.size()) - 1),
      regularTypes_{
          TileType::Bamboo,
          TileType::Circle,
          TileType::Flag,
          TileType::Bag,
          TileType::Earring,
      } {}

Tile TileSpawner::Spawn() {
    const int roll = rollType_(random_);
    if (roll < 12) {
        return Tile{TileType::Joker};
    }

    return Tile{regularTypes_[rollRegularType_(random_)]};
}

}  // namespace TileMatchingGame
