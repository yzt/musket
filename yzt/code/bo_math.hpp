#pragma once

#include <cmath>

using Real = float;

inline int Round (Real x) {return int(x + 0.5f);}
inline Real Sqr (Real x) {return x * x;}
inline Real Sqrt (Real x) {return ::sqrtf(x);}
inline Real Abs (Real x) {return ::fabsf(x);}
inline Real Min (Real x, Real y) {return y < x ? y : x;}
inline Real Max (Real x, Real y) {return y < x ? x : y;}
inline Real Clamp (Real v, Real min, Real max) {return Min(Max(min, v), max);}

struct Vec2 {
    Real x, y;
};

using Point = Vec2;

inline Vec2 operator - (Vec2 const & a) {return {-a.x, -a.y};}

inline Vec2 operator + (Vec2 const & a, Vec2 const & b) {return {a.x + b.x, a.y + b.y};}
inline Vec2 operator + (Vec2 const & a, Real v)         {return {a.x + v, a.y + v};}
inline Vec2 operator + (Real v, Vec2 const & a)         {return {v + a.x, v + a.y};}
inline Vec2 operator - (Vec2 const & a, Vec2 const & b) {return {a.x - b.x, a.y - b.y};}
inline Vec2 operator - (Vec2 const & a, Real v)         {return {a.x - v, a.y - v};}
inline Vec2 operator - (Real v, Vec2 const & a)         {return {v - a.x, v - a.y};}
inline Vec2 operator * (Vec2 const & a, Vec2 const & b) {return {a.x * b.x, a.y * b.y};}
inline Vec2 operator * (Vec2 const & a, Real v)         {return {a.x * v, a.y * v};}
inline Vec2 operator * (Real v, Vec2 const & a)         {return {v * a.x, v * a.y};}
inline Vec2 operator / (Vec2 const & a, Vec2 const & b) {return {a.x / b.x, a.y / b.y};}
inline Vec2 operator / (Vec2 const & a, Real v)         {return {a.x / v, a.y / v};}
inline Vec2 operator / (Real v, Vec2 const & a)         {return {v / a.x, v / a.y};}

inline Vec2 & operator += (Vec2 & a, Vec2 const & b) {a.x += b.x; a.y += b.y; return a;}
inline Vec2 & operator += (Vec2 & a, Real v)         {a.x += v; a.y += v; return a;}
inline Vec2 & operator -= (Vec2 & a, Vec2 const & b) {a.x -= b.x; a.y -= b.y; return a;}
inline Vec2 & operator -= (Vec2 & a, Real v)         {a.x -= v; a.y -= v; return a;}
inline Vec2 & operator *= (Vec2 & a, Vec2 const & b) {a.x *= b.x; a.y *= b.y; return a;}
inline Vec2 & operator *= (Vec2 & a, Real v)         {a.x *= v; a.y *= v; return a;}
inline Vec2 & operator /= (Vec2 & a, Vec2 const & b) {a.x /= b.x; a.y /= b.y; return a;}
inline Vec2 & operator /= (Vec2 & a, Real v)         {a.x /= v; a.y /= v; return a;}

inline Real Dot (Vec2 const & a, Vec2 const & b) {return a.x * b.x + a.y * b.y;}

inline Real LengthSq (Vec2 const & a) {return a.x * a.x + a.y * a.y;}
inline Real Length (Vec2 const & a) {return Sqrt(a.x * a.x + a.y * a.y);}
inline Vec2 Normalized (Vec2 const & a) {auto inv_len = 1 / Length(a); return {inv_len * a.x, inv_len * a.y};}

inline Vec2 Clamp (Vec2 v, Vec2 min, Vec2 max) {return {Clamp(v.x, min.x, max.x), Clamp(v.y, min.y, max.y)};}

struct SegmentCollisionResult {
    bool exists;
    Real ta, tb;
};

SegmentCollisionResult Collides_SegmentToSegment (Point A0, Point A1, Point B0, Point B1) {
    SegmentCollisionResult ret = {};
    auto D = A1 - A0;
    auto E = B1 - B0;
    auto d = E.x * D.y - D.x * E.y;
    if (d != 0) {
        auto P = B0 - A0;
        ret.exists = true;
        ret.ta = (E.x * P.y - E.y * P.x) / d;
        if (E.x == 0)
            ret.tb = (-P.y + ret.ta * D.y) / E.y;
        else
            ret.tb = (-P.x + ret.ta * D.x) / E.x;   // Can still be NaN or something.
    }
    return ret;
}
