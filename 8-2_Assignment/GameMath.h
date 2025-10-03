#pragma once
#include <algorithm>
#include <limits>
#include <math.h>   // real CRT header

struct Vec2 {
    float x{ 0 }, y{ 0 };
    Vec2() = default;
    Vec2(float xx, float yy) : x(xx), y(yy) {}
    Vec2 operator+(const Vec2& o) const { return { x + o.x, y + o.y }; }
    Vec2 operator-(const Vec2& o) const { return { x - o.x, y - o.y }; }
    Vec2 operator*(float s) const { return { x * s, y * s }; }
    Vec2& operator+=(const Vec2& o) { x += o.x; y += o.y; return *this; }
    Vec2& operator-=(const Vec2& o) { x -= o.x; y -= o.y; return *this; }
    Vec2& operator*=(float s) { x *= s; y *= s; return *this; }
    float& operator[](int i) { return i == 0 ? x : y; }
    const float& operator[](int i) const { return i == 0 ? x : y; }
};

inline float dot(const Vec2& a, const Vec2& b) { return a.x * b.x + a.y * b.y; }
inline float length(const Vec2& v) { return sqrtf(dot(v, v)); }
inline Vec2  normalize(const Vec2& v) {
    float L = length(v);
    return (L > 0.0f) ? Vec2{ v.x / L, v.y / L } : Vec2{ 0,0 };
}

// lightweight clamps (avoid std::clamp / C++17)
inline float clampf(float v, float lo, float hi) { return v < lo ? lo : (v > hi ? hi : v); }
inline int   clampi(int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); }

struct Transform { Vec2 position{ 0,0 }; Vec2 velocity{ 0,0 }; };
struct Health { int  hp{ 1 }; };

// Segment (p0->p1) vs AABB centered at 'center' with half-extents 'half'
inline bool segmentIntersectsAABB(const Vec2& p0, const Vec2& p1,
    const Vec2& center, const Vec2& half,
    float* tHitOut = nullptr, Vec2* nOut = nullptr)
{
    Vec2 d = { p1.x - p0.x, p1.y - p0.y };
    float t0 = 0.0f, t1 = 1.0f;
    Vec2 nHit{ 0,0 };

    for (int i = 0; i < 2; ++i) {
        float di = (i == 0 ? d.x : d.y);
        float invD = (di != 0.0f) ? 1.0f / di : std::numeric_limits<float>::infinity();
        float minB = (i == 0 ? center.x - half.x : center.y - half.y);
        float maxB = (i == 0 ? center.x + half.x : center.y + half.y);
        float p0i = (i == 0 ? p0.x : p0.y);

        float tNear = (minB - p0i) * invD;
        float tFar = (maxB - p0i) * invD;
        float sgn = -1.0f;
        if (tNear > tFar) { float tmp = tNear; tNear = tFar; tFar = tmp; sgn = 1.0f; }

        if (tNear > t0) {
            t0 = tNear;
            nHit = { 0,0 };
            if (i == 0) nHit.x = sgn; else nHit.y = sgn;
        }
        if (tFar < t1) t1 = tFar;
        if (t0 > t1) return false;
    }

    if (tHitOut) *tHitOut = t0;
    if (nOut) *nOut = nHit;
    return true;
}
