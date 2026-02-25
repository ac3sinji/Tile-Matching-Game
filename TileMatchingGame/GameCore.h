#pragma once

#include "BoardState.h"
#include "TileSpawner.h"

#include <optional>
#include <random>

namespace TileMatchingGame {

class GameCore {
public:
    GameCore();

    void StartLevel();
    void SpawnTurnTile();
    PlacementResult TryPlacePlayerTile(int row, int col);

    bool ShouldShowInvalidMoveFeedback() const;
    bool IsWaitingForPlayer() const;
    std::optional<Participant> GetWinner() const;
    std::optional<Tile> GetCurrentSpawnTile() const;
    TileType GetPlayerTargetType() const;
    TileType GetAITargetType() const;
    const BoardState& GetPlayerBoard() const;
    const BoardState& GetAIBoard() const;

private:
    void AssignTargets();
    void EvaluateWinner();

    BoardState playerBoard_;
    BoardState aiBoard_;
    TileSpawner spawner_;
    std::mt19937 random_;

    TileType playerTargetType_ = TileType::Bamboo;
    TileType aiTargetType_ = TileType::Circle;

    std::optional<Tile> currentSpawnTile_;
    std::optional<Participant> winner_;

    bool isWaitingForPlayer_ = false;
    bool showInvalidMoveFeedback_ = false;
};

}  // namespace TileMatchingGame
