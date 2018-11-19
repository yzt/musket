#include <cmath>

using Real = float;

struct Vec2f {
    Real x, y;
};

struct Vec2i {
    int x, y;
};

using Point2f = Vec2f;

inline Vec2f operator + (Vec2f const & v, Vec2f const & u) {return {v.x + u.x, v.y + u.y};}
inline Vec2f operator - (Vec2f const & v, Vec2f const & u) {return {v.x - u.x, v.y - u.y};}
inline Vec2f operator * (Vec2f const & v, Vec2f const & u) {return {v.x * u.x, v.y * u.y};}
inline Vec2f operator / (Vec2f const & v, Vec2f const & u) {return {v.x / u.x, v.y / u.y};}

inline Vec2f operator + (Vec2f const & v, Real x) {return {v.x + x, v.y + x};}
inline Vec2f operator - (Vec2f const & v, Real x) {return {v.x - x, v.y - x};}
inline Vec2f operator * (Vec2f const & v, Real x) {return {v.x * x, v.y * x};}
inline Vec2f operator / (Vec2f const & v, Real x) {return {v.x / x, v.y / x};}

inline Vec2f operator + (Real x, Vec2f const & u) {return {x + u.x, x + u.y};}
inline Vec2f operator - (Real x, Vec2f const & u) {return {x - u.x, x - u.y};}
inline Vec2f operator * (Real x, Vec2f const & u) {return {x * u.x, x * u.y};}
inline Vec2f operator / (Real x, Vec2f const & u) {return {x / u.x, x / u.y};}

inline Vec2f & operator += (Vec2f & v, Vec2f const & u) {v.x += u.x; v.y += u.y; return v;}
inline Vec2f & operator -= (Vec2f & v, Vec2f const & u) {v.x -= u.x; v.y -= u.y; return v;}
inline Vec2f & operator *= (Vec2f & v, Vec2f const & u) {v.x *= u.x; v.y *= u.y; return v;}
inline Vec2f & operator /= (Vec2f & v, Vec2f const & u) {v.x /= u.x; v.y /= u.y; return v;}

inline Vec2f & operator += (Vec2f & v, Real x) {v.x += x; v.y += x; return v;}
inline Vec2f & operator -= (Vec2f & v, Real x) {v.x -= x; v.y -= x; return v;}
inline Vec2f & operator *= (Vec2f & v, Real x) {v.x *= x; v.y *= x; return v;}
inline Vec2f & operator /= (Vec2f & v, Real x) {v.x /= x; v.y /= x; return v;}

inline int Abs (int x) {
    return ::abs(x);
}

inline Real Abs (Real x) {
    return ::abs(x);
}

inline Real Sqr (Real x) {
    return x * x;
}

inline Real Sqrt (Real x) {
    return ::sqrt(x);
}

inline int Round (Real x) {
    return int(x + 0.5f);
}

inline Real Sin (Real x) {
    return ::sin(x);
}

inline Real Cos (Real x) {
    return ::cos(x);
}

inline Real Dot (Vec2f const & v, Vec2f const & u) {
    return v.x * u.x + v.y * u.y;
}

inline Real LengthSq (Vec2f const & v) {
    return Sqr(v.x) + Sqr(v.y);
}

inline Real Length (Vec2f const & v) {
    return Sqrt(LengthSq(v));
}

inline Real InvLength (Vec2f const & v) {
    return 1.0f / Length(v);
}

inline Vec2f Normalize (Vec2f const & v) {
    return InvLength(v) * v;
}

inline bool AlmostZero (Real x, Real epsilon = 0.000001f) {
    return Abs(x) < epsilon;
}

inline Real Lerp (Real a, Real b, Real t) {
    return a + t * (b - a);
}

inline Vec2f Lerp (Vec2f const & a, Vec2f const & b, Real t) {
    return a + t * (b - a);
}

inline Vec2f Reflect (Vec2f const & incident, Vec2f const & unit_normal) {
    ASSERT(1 == LengthSq(unit_normal));
    return incident - 2 * Dot(incident, unit_normal) * unit_normal;
}

struct LineIntersectResult {
    bool exists;
    Real l_param, m_param;
};

inline LineIntersectResult Intersect_LineLine (
    Point2f const & l0, Point2f const & l1,
    Point2f const & m0, Point2f const & m1
) {
    LineIntersectResult ret = {};
    auto d = l1 - l0;
    auto e = m1 - m0;
    auto denom = (d.y * e.x) - (e.y * d.x);
    if (!AlmostZero(denom)) {
        auto inv_denom = 1.0f / denom;
        auto a = m0 - l0;
        ret.l_param = (e.x * a.y - e.y * a.x) * inv_denom;
        ret.m_param = (d.x * a.y - d.y * a.x) * inv_denom;
        ret.exists = true;
    }
    return ret;
}
