#include "TileDomain.h"

namespace TileMatchingGame {

bool IsRegularTile(TileType type) {
    return type != TileType::Joker;
}

const wchar_t* TileTypeToText(TileType type) {
    switch (type) {
    case TileType::Bamboo:
        return L"\uC0AD\uC218\uD328";
    case TileType::Circle:
        return L"\uD1B5\uC218\uD328";
    case TileType::Flag:
        return L"\uAD6D\uAE30";
    case TileType::Bag:
        return L"\uAC00\uBC29";
    case TileType::Earring:
        return L"\uADC0\uAC78\uC774";
    case TileType::Joker:
        return L"\uBB34\uC9C0\uAC1C \uC54C";
    default:
        return L"\uC54C \uC218 \uC5C6\uC74C";
    }
}

}  // namespace TileMatchingGame
