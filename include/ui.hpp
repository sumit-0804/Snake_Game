#pragma once

#include <vector>
#include <string>
#include "types.hpp"

void clearScreen();
int showMainMenu();
void showTopScores(const std::vector<PlayerScore> &scores);
std::string askPlayerName();
int showTitleAndMenu();
