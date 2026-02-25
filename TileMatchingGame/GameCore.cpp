#include "GameCore.h"

#include <array>

namespace TileMatchingGame {

GameCore::GameCore() : random_(std::random_device{}()) {
    StartLevel();
}

void GameCore::StartLevel() {
    playerBoard_ = BoardState{};
    aiBoard_ = BoardState{};
    currentSpawnTile_.reset();
    isWaitingForPlayer_ = false;
    showInvalidMoveFeedback_ = false;
    winner_.reset();

    AssignTargets();
}

void GameCore::SpawnTurnTile() {
    if (winner_.has_value() || isWaitingForPlayer_) {
        return;
    }

    for (int attempt = 0; attempt < 16; ++attempt) {
        Tile spawnedTile = spawner_.Spawn();

        if (spawnedTile.type == aiTargetType_) {
            aiBoard_.PlaceFirstEmpty(spawnedTile, aiTargetType_);
            EvaluateWinner();
            return;
        }

        if (spawnedTile.type == playerTargetType_ || spawnedTile.type == TileType::Joker) {
            currentSpawnTile_ = spawnedTile;
            isWaitingForPlayer_ = true;
            return;
        }

        if (!IsRegularTile(spawnedTile.type)) {
            continue;
        }
    }
}

PlacementResult GameCore::TryPlacePlayerTile(int row, int col) {
    showInvalidMoveFeedback_ = false;

    if (!currentSpawnTile_.has_value()) {
        return PlacementResult::NoSpawnTile;
    }

    const PlacementResult result = playerBoard_.PlaceTile(row, col, currentSpawnTile_.value(), playerTargetType_);
    if (result != PlacementResult::Success) {
        if (result == PlacementResult::InvalidTile) {
            showInvalidMoveFeedback_ = true;
        }
        return result;
    }

    currentSpawnTile_.reset();
    isWaitingForPlayer_ = false;
    EvaluateWinner();
    return PlacementResult::Success;
}

bool GameCore::ShouldShowInvalidMoveFeedback() const {
    return showInvalidMoveFeedback_;
}

bool GameCore::IsWaitingForPlayer() const {
    return isWaitingForPlayer_;
}

std::optional<Participant> GameCore::GetWinner() const {
    return winner_;
}

std::optional<Tile> GameCore::GetCurrentSpawnTile() const {
    return currentSpawnTile_;
}

TileType GameCore::GetPlayerTargetType() const {
    return playerTargetType_;
}

TileType GameCore::GetAITargetType() const {
    return aiTargetType_;
}

void GameCore::AssignTargets() {
    std::array<TileType, 5> regularTypes{
        TileType::Bamboo,
        TileType::Circle,
        TileType::Flag,
        TileType::Bag,
        TileType::Earring,
    };

    std::uniform_int_distribution<int> pickIndex(0, static_cast<int>(regularTypes.size()) - 1);
    const int playerIndex = pickIndex(random_);

    int aiIndex = pickIndex(random_);
    while (aiIndex == playerIndex) {
        aiIndex = pickIndex(random_);
    }

    playerTargetType_ = regularTypes[playerIndex];
    aiTargetType_ = regularTypes[aiIndex];
}

void GameCore::EvaluateWinner() {
    if (playerBoard_.IsFull()) {
        winner_ = Participant::Player;
        isWaitingForPlayer_ = false;
        currentSpawnTile_.reset();
        return;
    }

    if (aiBoard_.IsFull()) {
        winner_ = Participant::AI;
        isWaitingForPlayer_ = false;
        currentSpawnTile_.reset();
    }
}

}  // namespace TileMatchingGame
