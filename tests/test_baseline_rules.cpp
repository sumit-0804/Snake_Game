// Part B - the five rules, tested against tag `lab4-base` with no change to
// src/ or include/. Only R4 is testable as a rule. R1 and R5 survive as tests of
// the predicate underneath them. R2 and R3 are unreachable. Blockers are cited
// by file:line in Section 2 of the report.

#include "microtest.hpp"

#include "types.hpp"
#include "util.hpp"

#include <set>
#include <utility>

namespace
{

    GameConfig smallBoard()
    {
        GameConfig cfg;
        cfg.rows = 5;
        cfg.cols = 5;
        return cfg;
    }

} // namespace

// R1 - "the snake dies the moment its head leaves the board".
// Only inBounds() is reachable; the death half of the rule is not.

TEST(inBounds_accepts_every_corner_of_the_board)
{
    const GameConfig cfg = smallBoard();

    CHECK(inBounds(cfg, Vec{0, 0}), "the top-left cell is on the board");
    CHECK(inBounds(cfg, Vec{0, 4}), "the top-right cell is on the board");
    CHECK(inBounds(cfg, Vec{4, 0}), "the bottom-left cell is on the board");
    CHECK(inBounds(cfg, Vec{4, 4}), "the bottom-right cell is on the board");
}

TEST(inBounds_rejects_the_cell_just_past_each_edge)
{
    const GameConfig cfg = smallBoard();

    CHECK(!inBounds(cfg, Vec{-1, 2}), "one row above the top edge is off the board");
    CHECK(!inBounds(cfg, Vec{5, 2}), "one row below the bottom edge is off the board");
    CHECK(!inBounds(cfg, Vec{2, -1}), "one column left of the board is off the board");
    CHECK(!inBounds(cfg, Vec{2, 5}), "one column right of the board is off the board");
}

TEST(step_moves_the_head_exactly_one_cell_in_the_heading)
{
    const Vec origin{3, 3};

    CHECK_EQ(step(origin, UP).r, 2, "UP decreases the row by one");
    CHECK_EQ(step(origin, UP).c, 3, "UP leaves the column alone");
    CHECK_EQ(step(origin, DOWN).r, 4, "DOWN increases the row by one");
    CHECK_EQ(step(origin, LEFT).c, 2, "LEFT decreases the column by one");
    CHECK_EQ(step(origin, RIGHT).c, 4, "RIGHT increases the column by one");
}

// R4 - "fruit never appears on a cell the snake or an obstacle occupies".
// The only one of the five testable as written.

TEST(placeFood_never_lands_on_the_snake_or_an_obstacle)
{
    const GameConfig cfg = smallBoard();
    GameState st;
    st.snake.push_back(Vec{2, 2});
    st.snake.push_back(Vec{2, 1});
    st.snake.push_back(Vec{2, 0});
    st.obstacles.insert({0, 0});
    st.obstacles.insert({4, 4});

    // The generator is unseedable here, so the rule is checked as an invariant
    // over many draws rather than against one expected cell.
    for (int draw = 0; draw < 200; ++draw)
    {
        placeFood(cfg, st);

        bool onSnake = false;
        for (const Vec &segment : st.snake)
            if (segment.r == st.food.r && segment.c == st.food.c)
                onSnake = true;

        CHECK(!onSnake, "fruit must never be placed on a cell the snake occupies");
        CHECK(st.obstacles.count({st.food.r, st.food.c}) == 0,
              "fruit must never be placed on a cell an obstacle occupies");
        CHECK(inBounds(cfg, st.food), "fruit must always be placed on the board");
    }
}

TEST(placeFood_uses_the_only_free_cell_when_one_remains)
{
    const GameConfig cfg = smallBoard();
    GameState st;
    // Every cell occupied except {1, 3}.
    for (int r = 0; r < cfg.rows; ++r)
        for (int c = 0; c < cfg.cols; ++c)
            if (!(r == 1 && c == 3))
                st.obstacles.insert({r, c});

    placeFood(cfg, st);

    CHECK_EQ(st.food.r, 1, "the only free row is the one the fruit must take");
    CHECK_EQ(st.food.c, 3, "the only free column is the one the fruit must take");
    CHECK(!st.gameOver, "a board with a free cell left is not a finished board");
}

TEST(placeFood_ends_the_game_when_no_free_cell_is_left)
{
    const GameConfig cfg = smallBoard();
    GameState st;
    for (int r = 0; r < cfg.rows; ++r)
        for (int c = 0; c < cfg.cols; ++c)
            st.obstacles.insert({r, c});

    placeFood(cfg, st);

    CHECK(st.gameOver, "a board with nowhere left to place fruit ends the game");
}

// R5 - "an input exactly opposite to the current heading is ignored".
// isOpposite() is reachable; the rule is not, because runGame reads the heading
// from a file-static atomic written by a keyboard thread.

TEST(isOpposite_is_true_for_the_two_reversing_pairs)
{
    CHECK(isOpposite(UP, DOWN), "DOWN reverses UP");
    CHECK(isOpposite(DOWN, UP), "UP reverses DOWN");
    CHECK(isOpposite(LEFT, RIGHT), "RIGHT reverses LEFT");
    CHECK(isOpposite(RIGHT, LEFT), "LEFT reverses RIGHT");
}

TEST(isOpposite_is_false_for_turns_and_for_holding_the_heading)
{
    CHECK(!isOpposite(UP, LEFT), "a left turn out of UP is not a reversal");
    CHECK(!isOpposite(UP, RIGHT), "a right turn out of UP is not a reversal");
    CHECK(!isOpposite(UP, UP), "holding the heading is not a reversal");
    CHECK(!isOpposite(LEFT, UP), "a right turn out of LEFT is not a reversal");
}

// R2 and R3 have no tests. Both are decided inside the while loop of runGame,
// which cannot be entered without a keyboard thread, a frame timer and a live
// terminal. See Section 2 of the report.

int main()
{
    return microtest::run("Part B - rules testable at lab4-base");
}
