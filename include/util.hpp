#pragma once

#include "types.hpp"

int urand(int lo, int hi);
bool inBounds(const GameConfig &cfg, Vec v);
bool isOpposite(Dir a, Dir b);
Vec step(Vec v, Dir d);
FoodType determineFoodType(const GameConfig &cfg);
void placeFood(const GameConfig &cfg, GameState &st);
void buildObstacles(const GameConfig &cfg, GameState &st);
void calculateBoardSize(GameConfig &cfg);
