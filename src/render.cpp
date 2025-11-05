#include <iostream>
#include "render.hpp"

using namespace std;

string gotorc(int r, int c) { return "\x1b[" + to_string(r) + ";" + to_string(c) + "H"; }
string clear() { return "\x1b[2J"; }
string hideCursor() { return "\x1b[?25l"; }
string showCursor() { return "\x1b[?25h"; }

void drawCell(const GameConfig &cfg, int originRow, int originCol, Vec v, const string &glyph)
{
    (void)cfg;
    int r = originRow + v.r;
    int c = originCol + v.c * 2;
    cout << gotorc(r, c) << glyph;
}

void drawEmojiBorder(const GameConfig &cfg, int originRow, int originCol)
{
    for (int c = -1; c <= cfg.cols; ++c)
    {
        cout << gotorc(originRow - 1, originCol + c * 2) << WALL;
        cout << gotorc(originRow + cfg.rows, originCol + c * 2) << WALL;
    }
    for (int r = 0; r < cfg.rows; ++r)
    {
        cout << gotorc(originRow + r, originCol - 2) << WALL;
        cout << gotorc(originRow + r, originCol + cfg.cols * 2) << WALL;
    }
}

void drawScorePanel(const GameConfig &cfg, int originRow, int originCol, const GameState &st)
{
    int panelCol = originCol + cfg.cols * 2 + 5;

    cout << gotorc(originRow - 1, panelCol)
         << Color::BRIGHT_YELLOW << Color::BOLD << "╔═══════════════╗" << Color::RESET;
    cout << gotorc(originRow, panelCol)
         << Color::BRIGHT_YELLOW << Color::BOLD << "║   SNAKE 🐉    ║" << Color::RESET;
    cout << gotorc(originRow + 1, panelCol)
         << Color::BRIGHT_YELLOW << Color::BOLD << "╚═══════════════╝" << Color::RESET;

    cout << gotorc(originRow + 3, panelCol)
         << Color::BRIGHT_GREEN << "Score: " << Color::BRIGHT_WHITE << st.score << "   " << Color::RESET;
    cout << gotorc(originRow + 4, panelCol)
         << Color::BRIGHT_CYAN << "Length: " << Color::BRIGHT_WHITE << st.snake.size() << "   " << Color::RESET;
    cout << gotorc(originRow + 5, panelCol)
         << Color::BRIGHT_MAGENTA << "Moves: " << Color::BRIGHT_WHITE << st.moves << "   " << Color::RESET;
    cout << gotorc(originRow + 6, panelCol)
         << Color::BRIGHT_YELLOW << "Golden: " << Color::BRIGHT_WHITE << st.goldenApplesEaten << "   " << Color::RESET;

    cout << gotorc(originRow + 8, panelCol)
         << Color::YELLOW << Color::UNDERLINE << "Controls:" << Color::RESET;
    cout << gotorc(originRow + 9, panelCol) << Color::WHITE << "↑ W: Up" << Color::RESET;
    cout << gotorc(originRow + 10, panelCol) << Color::WHITE << "↓ S: Down" << Color::RESET;
    cout << gotorc(originRow + 11, panelCol) << Color::WHITE << "← A: Left" << Color::RESET;
    cout << gotorc(originRow + 12, panelCol) << Color::WHITE << "→ D: Right" << Color::RESET;
    cout << gotorc(originRow + 13, panelCol) << Color::RED << "Q: Quit" << Color::RESET;

    cout << gotorc(originRow + 15, panelCol)
         << Color::DIM << Color::CYAN << "🍎 +10, +1 seg" << Color::RESET;
    cout << gotorc(originRow + 16, panelCol)
         << Color::DIM << Color::YELLOW << "⭐ +30, +2 seg!" << Color::RESET;
    cout << gotorc(originRow + 17, panelCol)
         << Color::DIM << Color::CYAN << "Avoid walls 🪨" << Color::RESET;

    cout << gotorc(originRow + 19, panelCol)
         << Color::DIM << Color::WHITE << "Board: " << cfg.rows << "x" << cfg.cols << "   " << Color::RESET;
}

void drawBoardInitial(const GameConfig &cfg, const GameState &st, int originRow, int originCol)
{
    cout << clear() << hideCursor();
    drawEmojiBorder(cfg, originRow, originCol);

    for (int r = 0; r < cfg.rows; ++r)
    {
        for (int c = 0; c < cfg.cols; ++c)
        {
            cout << gotorc(originRow + r, originCol + c * 2) << EMPTY;
        }
    }
    for (auto &o : st.obstacles)
    {
        drawCell(cfg, originRow, originCol, {o.first, o.second}, OBSTACLE);
    }
}

void drawScreenBorder(int rows, int cols, int left, int top)
{
    for (int c = -1; c <= cols; ++c)
    {
        cout << gotorc(top - 1, left + c * 2) << WALL;
        cout << gotorc(top + rows, left + c * 2) << WALL;
    }
    for (int r = 0; r < rows; ++r)
    {
        cout << gotorc(top + r, left - 2) << WALL;
        cout << gotorc(top + r, left + cols * 2) << WALL;
    }
}
