#pragma once
#include <vector>
#include "Brick.h"

// Build a simple Brick Breaker grid: top rows are steel (reflective),
// below are one-hit destructible bricks.
inline void buildLevel(std::vector<Brick>& out) {
    out.clear();
    const int rows = 6;
    const int cols = 10;
    const float startY = 0.8f;
    const float rowH = 0.08f;
    const float padX = 0.02f;
    const float totalW = 1.8f;               // from -0.9 to 0.9
    const float cellW = totalW / cols;
    const float w = cellW - padX;             // brick width
    const float h = rowH * 0.8f;

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            float cx = -0.9f + cellW * c + cellW / 2.f;
            float cy = startY - r * rowH;

            bool steel = (r < 2); // top 2 rows are unbreakable
            if (steel) {
                out.emplace_back(BRICKTYPE::REFLECTIVE, cx, cy, w, h, 0.7f, 0.7f, 0.8f, 999);
            }
            else {
                // color bands per row
                float rr = 0.2f + 0.15f * r;
                float gg = 0.5f + 0.08f * r;
                float bb = 1.0f - 0.1f * r;
                out.emplace_back(BRICKTYPE::DESTRUCTABLE, cx, cy, w, h, rr, gg, bb, 1);
            }
        }
    }
}
