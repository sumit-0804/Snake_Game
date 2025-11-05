#include <random>
#include <vector>
#include <algorithm>
#include "util.hpp"
#include "platform.hpp"

int urand(int lo, int hi)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(lo, hi);
    return dist(gen);
}

bool inBounds(const GameConfig &cfg, Vec v)
{
    return v.r >= 0 && v.r < cfg.rows && v.c >= 0 && v.c < cfg.cols;
}

bool isOpposite(Dir a, Dir b)
{
    return (a == UP && b == DOWN) || (a == DOWN && b == UP) || (a == LEFT && b == RIGHT) || (a == RIGHT && b == LEFT);
}

Vec step(Vec v, Dir d)
{
    if (d == UP)
        v.r -= 1;
    if (d == DOWN)
        v.r += 1;
    if (d == LEFT)
        v.c -= 1;
    if (d == RIGHT)
        v.c += 1;
    return v;
}

FoodType determineFoodType(const GameConfig &cfg)
{
    int chance = urand(1, 100);
    if (chance <= cfg.goldenAppleChance)
    {
        return GOLDEN;
    }
    return NORMAL;
}

void placeFood(const GameConfig &cfg, GameState &st)
{
    std::vector<std::pair<int, int>> freeCells;
    std::vector<std::vector<bool>> occ(cfg.rows, std::vector<bool>(cfg.cols, false));
    for (auto &p : st.snake)
        if (inBounds(cfg, p))
            occ[p.r][p.c] = true;
    for (auto &o : st.obstacles)
        occ[o.first][o.second] = true;
    for (int r = 0; r < cfg.rows; r++)
        for (int c = 0; c < cfg.cols; c++)
            if (!occ[r][c])
                freeCells.push_back({r, c});
    if (freeCells.empty())
    {
        st.gameOver = true;
        return;
    }
    auto rc = freeCells[urand(0, (int)freeCells.size() - 1)];
    st.food = {rc.first, rc.second};
    st.foodType = determineFoodType(cfg);
}

void buildObstacles(const GameConfig &cfg, GameState &st)
{
    int numObstacleGroups = urand(3, std::min(12, cfg.cols));
    for (int i = 0; i < numObstacleGroups; ++i)
    {
        int pattern = urand(0, 2);
        if (pattern == 0)
        {
            int row = urand(2, cfg.rows - 3);
            int startCol = urand(2, cfg.cols / 2);
            int length = urand(3, std::min(6, cfg.cols / 4));
            for (int c = startCol; c < std::min(cfg.cols - 2, startCol + length); c += 2)
            {
                st.obstacles.insert({row, c});
            }
        }
        else if (pattern == 1)
        {
            int col = urand(3, cfg.cols - 3);
            int startRow = urand(2, cfg.rows / 2);
            int length = urand(3, std::min(5, cfg.rows / 4));
            for (int r = startRow; r < std::min(cfg.rows - 2, startRow + length); r += 2)
            {
                st.obstacles.insert({r, col});
            }
        }
        else
        {
            int centerRow = urand(3, cfg.rows - 4);
            int centerCol = urand(3, cfg.cols - 4);
            int clusterSize = urand(2, 4);
            for (int j = 0; j < clusterSize; ++j)
            {
                int offsetR = urand(-2, 2);
                int offsetC = urand(-2, 2);
                int r = centerRow + offsetR;
                int c = centerCol + offsetC;
                if (r >= 1 && r < cfg.rows - 1 && c >= 1 && c < cfg.cols - 1)
                {
                    st.obstacles.insert({r, c});
                }
            }
        }
    }
    int sr = cfg.rows / 2;
    int sc = cfg.cols / 2;
    for (int r = sr - 2; r <= sr + 2; ++r)
    {
        for (int c = sc - 4; c <= sc + 4; ++c)
        {
            st.obstacles.erase({r, c});
        }
    }
}

void calculateBoardSize(GameConfig &cfg)
{
    auto rc = getTerminalSize();
    int termRows = rc.first;
    int termCols = rc.second;
    int availableRows = termRows - 6;
    int availableCols = (termCols - 30) / 2;
    cfg.rows = std::max(12, std::min(availableRows, 30));
    cfg.cols = std::max(20, std::min(availableCols, 45));
    if (cfg.rows % 2 == 0)
        cfg.rows--;
    if (cfg.cols % 2 == 0)
        cfg.cols--;
}
