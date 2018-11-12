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

inline Real Sqr (Real x) {
    return x * x;
}

inline Real Sqrt (Real x) {
    return ::sqrtf(x);
}

inline int Round (Real x) {
    return int(x + 0.5f);
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
