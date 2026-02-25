#include "TileDomain.h"

namespace TileMatchingGame {

bool IsRegularTile(TileType type) {
    return type != TileType::Joker;
}

const wchar_t* TileTypeToText(TileType type) {
    switch (type) {
    case TileType::Bamboo:
        return L"삭수패";
    case TileType::Circle:
        return L"통수패";
    case TileType::Flag:
        return L"국기";
    case TileType::Bag:
        return L"가방";
    case TileType::Earring:
        return L"귀걸이";
    case TileType::Joker:
        return L"무지개 알";
    default:
        return L"알 수 없음";
    }
}

}  // namespace TileMatchingGame
