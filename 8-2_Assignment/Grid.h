#pragma once
#include <vector>
#include <utility>
#include <math.h>
#include "GameMath.h"

// Uniform grid on NDC space [-1,1]x[-1,1]
class UniformGrid {
public:
    UniformGrid(int rows, int cols) : rows_(rows), cols_(cols), cells_(rows* cols) {}
    void clear() { for (size_t i = 0; i < cells_.size(); ++i) cells_[i].clear(); }

    void insert(int id, const Vec2& center, const Vec2& half) {
        std::pair<int, int> p0 = cellFromPoint(Vec2{ center.x - half.x, center.y - half.y });
        std::pair<int, int> p1 = cellFromPoint(Vec2{ center.x + half.x, center.y + half.y });
        int r0 = clampi(p0.first, 0, rows_ - 1);
        int c0 = clampi(p0.second, 0, cols_ - 1);
        int r1 = clampi(p1.first, 0, rows_ - 1);
        int c1 = clampi(p1.second, 0, cols_ - 1);

        for (int r = r0; r <= r1; ++r)
            for (int c = c0; c <= c1; ++c)
                cells_[r * cols_ + c].push_back(id);
    }

    void query(const Vec2& center, const Vec2& half, std::vector<int>& outIds) const {
        outIds.clear();
        std::pair<int, int> q0 = cellFromPoint(Vec2{ center.x - half.x, center.y - half.y });
        std::pair<int, int> q1 = cellFromPoint(Vec2{ center.x + half.x, center.y + half.y });
        int r0 = clampi(q0.first, 0, rows_ - 1);
        int c0 = clampi(q0.second, 0, cols_ - 1);
        int r1 = clampi(q1.first, 0, rows_ - 1);
        int c1 = clampi(q1.second, 0, cols_ - 1);

        for (int r = r0; r <= r1; ++r)
            for (int c = c0; c <= c1; ++c) {
                const std::vector<int>& cell = cells_[r * cols_ + c];
                outIds.insert(outIds.end(), cell.begin(), cell.end());
            }
        // dedupe (simple bubble for small sets)
        std::sort(outIds.begin(), outIds.end());
        outIds.erase(std::unique(outIds.begin(), outIds.end()), outIds.end());
    }

private:
    std::pair<int, int> cellFromPoint(const Vec2& p) const {
        float u = (p.x + 1.0f) * 0.5f * (float)cols_;
        float v = (p.y + 1.0f) * 0.5f * (float)rows_;
        int col = (int)floorf(u);
        int row = (int)floorf(v);
        return std::make_pair(row, col);
    }

    int rows_, cols_;
    std::vector<std::vector<int>> cells_;
};
