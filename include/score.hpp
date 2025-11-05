#pragma once

#include <string>
#include <vector>
#include "types.hpp"

// Returns platform-appropriate default path for scores file and ensures parent dir exists.
std::string getDefaultScoresPath();

// Load/save scores from/to file. Silently ignores errors and returns best-effort results.
std::vector<PlayerScore> loadScores(const std::string &path);
bool saveScores(const std::string &path, const std::vector<PlayerScore> &scores);

// Insert score, sort descending, keep top 10.
void addScore(std::vector<PlayerScore> &scores, const std::string &name, int score);
