#pragma once

#include <string>
#include "types.hpp"
#include "color.hpp"

// Cursor/terminal helpers
std::string gotorc(int r, int c);
std::string clear();
std::string hideCursor();
std::string showCursor();

// Glyphs
inline const std::string SNAKE_HEAD = Color::BRIGHT_GREEN + "🐍" + Color::RESET;
inline const std::string SNAKE_BODY = Color::BRIGHT_GREEN + "🐍" + Color::RESET;
inline const std::string FOOD_NORMAL = Color::BRIGHT_RED + "🍎" + Color::RESET;
inline const std::string FOOD_GOLDEN = Color::BRIGHT_YELLOW + "⭐" + Color::RESET;
inline const std::string OBSTACLE = Color::fg(240) + "🪨" + Color::RESET;
inline const std::string EMPTY = Color::bg(235) + "  " + Color::RESET;
inline const std::string WALL = Color::BRIGHT_CYAN + "⬛" + Color::RESET;

void drawCell(const GameConfig &cfg, int originRow, int originCol, Vec v, const std::string &glyph);
void drawEmojiBorder(const GameConfig &cfg, int originRow, int originCol);
void drawScorePanel(const GameConfig &cfg, int originRow, int originCol, const GameState &st);
void drawBoardInitial(const GameConfig &cfg, const GameState &st, int originRow, int originCol);
void drawScreenBorder(int rows, int cols, int left, int top);
