#pragma once

#include "types.hpp"
#include <vector>

bool showGameOverScreen(const GameConfig &cfg, int originRow, int originCol, const GameState &st, const std::vector<PlayerScore> &topScores);
void initGameState(const GameConfig &cfg, GameState &st);
void runGame(const GameConfig &cfg, GameState &st, int originRow, int originCol);
