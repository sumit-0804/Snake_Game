#pragma once

#include <deque>
#include <set>
#include <string>

struct Vec
{
    int r, c;
};
enum Dir
{
    UP = 0,
    RIGHT = 1,
    DOWN = 2,
    LEFT = 3
};
enum FoodType
{
    NORMAL,
    GOLDEN
};

struct GameConfig
{
    int rows = 18;
    int cols = 24;
    int fps = 8;
    int scorePanelWidth = 20;
    int goldenAppleChance = 15;
    int normalAppleScore = 10;
    int goldenAppleScore = 30;
};

struct GameState
{
    std::deque<Vec> snake;
    Dir dir = RIGHT;
    Vec food{-1, -1};
    FoodType foodType = NORMAL;
    std::set<std::pair<int, int>> obstacles;
    bool gameOver = false;
    int score = 0;
    bool quit = false;
    int moves = 0;
    int goldenApplesEaten = 0;
    int segmentsToGrow = 0;
};

struct PlayerScore
{
    std::string name;
    int score;
};
