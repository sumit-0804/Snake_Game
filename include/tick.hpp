#pragma once

#include "types.hpp"
#include "rng.hpp"

// What one tick did, in the terms the renderer needs.
struct TickResult
{
    // False when the tick ended in a death before the snake moved.
    bool advanced = false;
    bool ate = false;
    bool tailVacated = false;
    Vec vacatedTail{-1, -1};
};

// Advances the game by one tick using st.dir. No input, no clock, no terminal:
// the caller owns all three. rng is only consulted when eating forces a respawn.
TickResult advanceTick(const GameConfig &cfg, GameState &st, Rng &rng = systemRng());
