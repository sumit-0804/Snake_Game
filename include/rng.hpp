#pragma once

// Seam for randomness. Replaces the file-static mt19937 that nothing outside
// util.cpp could reach, so tests can supply their own draws.
struct Rng
{
    virtual ~Rng() = default;

    // Inclusive range [lo, hi].
    virtual int next(int lo, int hi) = 0;
};

Rng &systemRng();
