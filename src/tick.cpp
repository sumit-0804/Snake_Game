#include "tick.hpp"
#include "util.hpp"

// Lifted out of the runGame() loop unchanged. The drawCell() call that sat in
// the tail-popping branch is reported through TickResult::tailVacated instead.
TickResult advanceTick(const GameConfig &cfg, GameState &st, Rng &rng)
{
    TickResult result;

    Vec newHead = step(st.snake.front(), st.dir);
    if (!inBounds(cfg, newHead))
        st.gameOver = true;
    if (!st.gameOver && st.obstacles.count({newHead.r, newHead.c}))
        st.gameOver = true;
    if (!st.gameOver)
    {
        for (auto &seg : st.snake)
            if (seg.r == newHead.r && seg.c == newHead.c)
            {
                st.gameOver = true;
                break;
            }
    }
    if (st.gameOver)
        return result; // was `break` in the original loop

    result.advanced = true;

    Vec tail = st.snake.back();
    st.snake.push_front(newHead);
    st.moves++;
    result.ate = (newHead.r == st.food.r && newHead.c == st.food.c);
    if (result.ate)
    {
        if (st.foodType == GOLDEN)
        {
            st.score += cfg.goldenAppleScore;
            st.goldenApplesEaten++;
            st.segmentsToGrow += 2;
        }
        else
        {
            st.score += cfg.normalAppleScore;
            st.segmentsToGrow += 1;
        }
        placeFood(cfg, st, rng);
    }
    if (st.segmentsToGrow > 0)
    {
        st.segmentsToGrow--;
    }
    else
    {
        st.snake.pop_back();
        result.tailVacated = true;
        result.vacatedTail = tail;
    }

    return result;
}
