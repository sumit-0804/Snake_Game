// Part D - the test the seam bought. The rule: eating an apple adds exactly that
// apple's score and grows the snake by exactly that apple's segments. At tag
// `lab4-base` it lived inside the runGame loop and no test could reach it.

#include "microtest.hpp"

#include "rng.hpp"
#include "tick.hpp"
#include "types.hpp"
#include "util.hpp"

#include <deque>
#include <utility>

namespace
{

    // The test double handed through the seam: a stub. It answers next() from a
    // script and records nothing.
    class ScriptedRng : public Rng
    {
    public:
        explicit ScriptedRng(std::deque<int> script) : script_(std::move(script)) {}

        int next(int lo, int /*hi*/) override
        {
            if (script_.empty())
                return lo;
            const int answer = script_.front();
            script_.pop_front();
            return answer;
        }

    private:
        std::deque<int> script_;
    };

    // placeFood collects free cells row by row, so 0 means the first free cell.
    constexpr int kFirstFreeCell = 0;

    // determineFoodType rolls 1..100 against goldenAppleChance, which is 15.
    constexpr int kRollBelowGoldenChance = 1;
    constexpr int kRollAboveGoldenChance = 100;

    GameConfig fiveByFiveBoard()
    {
        GameConfig cfg;
        cfg.rows = 5;
        cfg.cols = 5;
        return cfg;
    }

    // Three cells along row 2, head at {2, 2}, facing right. Each test places
    // its own fruit and obstacles.
    GameState snakeFacingRightAtRow2()
    {
        GameState st;
        st.snake.push_back(Vec{2, 2});
        st.snake.push_back(Vec{2, 1});
        st.snake.push_back(Vec{2, 0});
        st.dir = RIGHT;
        return st;
    }

} // namespace

TEST(eating_a_normal_apple_adds_its_score_and_exactly_one_segment)
{
    const GameConfig cfg = fiveByFiveBoard();
    GameState st = snakeFacingRightAtRow2();
    st.food = Vec{2, 3}; // directly ahead of the head
    st.foodType = NORMAL;
    ScriptedRng rng({kFirstFreeCell, kRollAboveGoldenChance});

    const TickResult tick = advanceTick(cfg, st, rng);

    CHECK(tick.ate, "moving onto the fruit cell counts as eating it");
    CHECK_EQ(st.score, cfg.normalAppleScore,
             "a normal apple adds exactly the configured normal apple score");
    CHECK_EQ((int)st.snake.size(), 4,
             "a normal apple grows a three-cell snake to exactly four cells");
    CHECK(!tick.tailVacated,
          "the tail is kept on the tick a normal apple is eaten, which is how the snake grows");
    CHECK_EQ(st.segmentsToGrow, 0,
             "one segment of growth is spent on the tick the apple is eaten");
}

TEST(eating_a_golden_apple_adds_its_score_and_exactly_two_segments)
{
    const GameConfig cfg = fiveByFiveBoard();
    GameState st = snakeFacingRightAtRow2();
    st.food = Vec{2, 3};
    st.foodType = GOLDEN;
    ScriptedRng rng({kFirstFreeCell, kRollAboveGoldenChance});

    advanceTick(cfg, st, rng);

    CHECK_EQ(st.score, cfg.goldenAppleScore,
             "a golden apple adds exactly the configured golden apple score");
    CHECK_EQ(st.goldenApplesEaten, 1, "a golden apple is counted as one golden apple");
    CHECK_EQ((int)st.snake.size(), 4,
             "the first of the two golden segments arrives on the eating tick");
    CHECK_EQ(st.segmentsToGrow, 1, "one golden segment is still owed after the eating tick");

    const TickResult next = advanceTick(cfg, st, rng);

    CHECK_EQ((int)st.snake.size(), 5,
             "the second golden segment arrives on the following tick, for two in total");
    CHECK(!next.tailVacated, "the tail is kept again while growth is still owed");
    CHECK_EQ(st.segmentsToGrow, 0, "the golden growth debt is settled after two ticks");
    CHECK_EQ(st.score, cfg.goldenAppleScore,
             "the tick after eating adds no further score");
}

TEST(the_score_added_is_the_configured_score_and_not_a_hard_coded_ten)
{
    GameConfig cfg = fiveByFiveBoard();
    cfg.normalAppleScore = 7;
    GameState st = snakeFacingRightAtRow2();
    st.food = Vec{2, 3};
    st.foodType = NORMAL;
    ScriptedRng rng({kFirstFreeCell, kRollAboveGoldenChance});

    advanceTick(cfg, st, rng);

    CHECK_EQ(st.score, 7, "the score added comes from the config, not from a literal");
}

TEST(a_tick_that_eats_nothing_leaves_the_score_and_the_length_alone)
{
    const GameConfig cfg = fiveByFiveBoard();
    GameState st = snakeFacingRightAtRow2();
    st.food = Vec{0, 0}; // nowhere near the head
    st.foodType = NORMAL;
    ScriptedRng rng({});

    const TickResult tick = advanceTick(cfg, st, rng);

    CHECK(!tick.ate, "a tick that does not reach the fruit does not eat");
    CHECK_EQ(st.score, 0, "moving without eating scores nothing");
    CHECK_EQ((int)st.snake.size(), 3, "moving without eating leaves the length unchanged");
    CHECK(tick.tailVacated, "the tail is released on a tick with no growth owed");
    CHECK_EQ(tick.vacatedTail.c, 0, "the released cell is the old tail at {2, 0}");
    CHECK_EQ(tick.vacatedTail.r, 2, "the released cell is the old tail at {2, 0}");
    CHECK_EQ(st.moves, 1, "a tick that advances the snake counts as one move");
}

TEST(eating_respawns_the_fruit_on_a_cell_the_snake_does_not_occupy)
{
    const GameConfig cfg = fiveByFiveBoard();
    GameState st = snakeFacingRightAtRow2();
    st.food = Vec{2, 3};
    st.foodType = NORMAL;
    ScriptedRng rng({kFirstFreeCell, kRollAboveGoldenChance});

    advanceTick(cfg, st, rng);

    CHECK_EQ(st.food.r, 0, "the stub sent the replacement fruit to the first free cell {0, 0}");
    CHECK_EQ(st.food.c, 0, "the stub sent the replacement fruit to the first free cell {0, 0}");
    bool onSnake = false;
    for (const Vec &segment : st.snake)
        if (segment.r == st.food.r && segment.c == st.food.c)
            onSnake = true;
    CHECK(!onSnake, "the replacement fruit is never placed under the snake");
}

// Closes the verification gap from Section 3: determineFoodType ran 201 times
// under the Part B suite, both branches taken, and nothing asserted the result.

TEST(a_roll_at_the_golden_chance_yields_a_golden_apple)
{
    GameConfig cfg = fiveByFiveBoard();
    cfg.goldenAppleChance = 15;
    ScriptedRng rng({15});

    CHECK(determineFoodType(cfg, rng) == GOLDEN,
          "a roll equal to the golden chance is inside it, so the apple is golden");
}

TEST(a_roll_one_past_the_golden_chance_yields_a_normal_apple)
{
    GameConfig cfg = fiveByFiveBoard();
    cfg.goldenAppleChance = 15;
    ScriptedRng rng({16});

    CHECK(determineFoodType(cfg, rng) == NORMAL,
          "a roll one past the golden chance is outside it, so the apple is normal");
}

TEST(a_golden_chance_of_zero_never_yields_a_golden_apple)
{
    GameConfig cfg = fiveByFiveBoard();
    cfg.goldenAppleChance = 0;
    ScriptedRng rng({kRollBelowGoldenChance});

    CHECK(determineFoodType(cfg, rng) == NORMAL,
          "a zero golden chance yields a normal apple even on the lowest roll");
}

// R1 and R2, blocked at lab4-base for the same reason and unlocked by the same seam.

TEST(the_snake_dies_when_its_head_leaves_the_board)
{
    const GameConfig cfg = fiveByFiveBoard();
    GameState st = snakeFacingRightAtRow2();
    st.snake.front() = Vec{0, 2}; // on the top row
    st.dir = UP;
    st.food = Vec{4, 4};
    ScriptedRng rng({});

    const TickResult tick = advanceTick(cfg, st, rng);

    CHECK(st.gameOver, "a head stepping off the top edge ends the game");
    CHECK(!tick.advanced, "a fatal tick does not advance the snake");
}

TEST(the_snake_dies_when_its_head_enters_its_own_body)
{
    const GameConfig cfg = fiveByFiveBoard();
    GameState st;
    // Hooked, so {3, 2} below the head is a middle segment and not the tail.
    st.snake.push_back(Vec{2, 2});
    st.snake.push_back(Vec{2, 3});
    st.snake.push_back(Vec{3, 3});
    st.snake.push_back(Vec{3, 2});
    st.snake.push_back(Vec{4, 2});
    st.dir = DOWN;
    st.food = Vec{0, 0};
    ScriptedRng rng({});

    const TickResult tick = advanceTick(cfg, st, rng);

    CHECK(st.gameOver, "a head entering a cell its own body occupies ends the game");
    CHECK(!tick.advanced, "a fatal tick does not advance the snake");
}

TEST(the_snake_dies_when_its_head_enters_an_obstacle)
{
    const GameConfig cfg = fiveByFiveBoard();
    GameState st = snakeFacingRightAtRow2();
    st.food = Vec{0, 0};
    st.obstacles.insert({2, 3}); // directly ahead of the head
    ScriptedRng rng({});

    const TickResult tick = advanceTick(cfg, st, rng);

    CHECK(st.gameOver, "a head entering an obstacle ends the game");
    CHECK(!tick.advanced, "a fatal tick does not advance the snake");
}

TEST(a_fatal_tick_changes_nothing_but_the_game_over_flag)
{
    const GameConfig cfg = fiveByFiveBoard();
    GameState st = snakeFacingRightAtRow2();
    st.snake.front() = Vec{2, 4}; // against the right edge
    st.food = Vec{0, 0};
    st.score = 40;
    st.moves = 9;
    ScriptedRng rng({});

    advanceTick(cfg, st, rng);

    CHECK_EQ((int)st.snake.size(), 3, "death leaves the snake at the length it had");
    CHECK_EQ(st.score, 40, "death does not change the score");
    CHECK_EQ(st.moves, 9, "a tick that kills the snake is not counted as a move");
}

int main()
{
    return microtest::run("Part D - the eating rule, through the seam");
}
