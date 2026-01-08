#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <cmath>

#include "define.hpp"

namespace mia {

template <typename T>
union Vec2 {
    struct {
        T x, y;
    };
    T items[2];
};

using Vec2i8 = Vec2<i8>;
using Vec2u8 = Vec2<u8>;
using Vec2i16 = Vec2<i16>;
using Vec2u16 = Vec2<u16>;
using Vec2i32 = Vec2<i32>;
using Vec2u32 = Vec2<u32>;
using Vec2i64 = Vec2<i64>;
using Vec2u64 = Vec2<u64>;
using Vec2f32 = Vec2<f32>;
using Vec2f64 = Vec2<f64>;

#ifndef VECTOR_HIGH_PRECISION
using Vec2f = Vec2<f32>;
using Vec2i = Vec2<i32>;
using Vec2u = Vec2<u32>;
#else
using Vec2f = Vec2<f64>;
using Vec2i = Vec2<i64>;
using Vec2u = Vec2<u64>;
#endif

template <typename T>
union Vec3 {
    struct {
        T x, y, z;
    };
    T items[3];
};

using Vec3i8 = Vec3<i8>;
using Vec3u8 = Vec3<u8>;
using Vec3i16 = Vec3<i16>;
using Vec3u16 = Vec3<u16>;
using Vec3i32 = Vec3<i32>;
using Vec3u32 = Vec3<u32>;
using Vec3i64 = Vec3<i64>;
using Vec3u64 = Vec3<u64>;
using Vec3f32 = Vec3<f32>;
using Vec3f64 = Vec3<f64>;

#ifndef VECTOR_HIGH_PRECISION
using Vec3f = Vec3<f32>;
using Vec3i = Vec3<i32>;
using Vec3u = Vec3<u32>;
#else
using Vec3f = Vec3<f64>;
using Vec3i = Vec3<i64>;
using Vec3u = Vec3<u64>;
#endif

#ifndef VECTOR_HIGH_PRECISION
using vector_scalar_t = f32;
#else
using vector_scalar_t = f64;
#endif

template <typename T>
Vec2<T> operator+(const Vec2<T> &a, const Vec2<T> &b) {
    return {a.x + b.x, a.y + b.y};
}

template <typename T>
Vec2<T> operator-(const Vec2<T> &a, const Vec2<T> &b) {
    return {a.x - b.x, a.y - b.y};
}

template <typename T>
Vec2<T> operator*(const Vec2<T> &v, vector_scalar_t scalar) {
    return {v.x * scalar, v.y * scalar};
}

template <typename T>
Vec2<T> operator*(vector_scalar_t scalar, const Vec2<T> &v) {
    return {v.x * scalar, v.y * scalar};
}

template <typename T>
Vec2<T> operator/(const Vec2<T> &v, vector_scalar_t scalar) {
    return {v.x / scalar, v.y / scalar};
}

template <typename T>
Vec2<T> &operator+=(Vec2<T> &a, const Vec2<T> &b) {
    a.x += b.x;
    a.y += b.y;
    return a;
}

template <typename T>
Vec2<T> &operator-=(Vec2<T> &a, const Vec2<T> &b) {
    a.x -= b.x;
    a.y -= b.y;
    return a;
}

template <typename T>
Vec2<T> &operator*=(Vec2<T> &v, vector_scalar_t scalar) {
    v.x *= scalar;
    v.y *= scalar;
    return v;
}

template <typename T>
Vec2<T> &operator/=(Vec2<T> &v, vector_scalar_t scalar) {
    v.x /= scalar;
    v.y /= scalar;
    return v;
}

template <typename T>
bool operator==(const Vec2<T> &a, const Vec2<T> &b) {
    return a.x == b.x && a.y == b.y;
}

template <typename T>
bool operator!=(const Vec2<T> &a, const Vec2<T> &b) {
    return !(a == b);
}

template <typename T>
vector_scalar_t dot(const Vec2<T> &a, const Vec2<T> &b) {
    return a.x * b.x + a.y * b.y;
}

template <typename T>
vector_scalar_t length_squared(const Vec2<T> &v) {
    return v.x * v.x + v.y * v.y;
}

template <typename T>
vector_scalar_t length(const Vec2<T> &v) {
    return sqrt(length_squared(v));
}

template <typename T>
Vec2<T> normalize(const Vec2<T> &v) {
    vector_scalar_t len = length(v);
    return v / len;
}

template <typename T>
vector_scalar_t distance(const Vec2<T> &a, const Vec2<T> &b) {
    return length(a - b);
}

template <typename T>
vector_scalar_t distance_squared(const Vec2<T> &a, const Vec2<T> &b) {
    return length_squared(a - b);
}

template <typename T>
Vec3<T> operator+(const Vec3<T> &a, const Vec3<T> &b) {
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

template <typename T>
Vec3<T> operator-(const Vec3<T> &a, const Vec3<T> &b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

template <typename T>
Vec3<T> operator*(const Vec3<T> &v, vector_scalar_t scalar) {
    return {v.x * scalar, v.y * scalar, v.z * scalar};
}

template <typename T>
Vec3<T> operator*(vector_scalar_t scalar, const Vec3<T> &v) {
    return {v.x * scalar, v.y * scalar, v.z * scalar};
}

template <typename T>
Vec3<T> operator/(const Vec3<T> &v, vector_scalar_t scalar) {
    return {v.x / scalar, v.y / scalar, v.z / scalar};
}

template <typename T>
Vec3<T> &operator+=(Vec3<T> &a, const Vec3<T> &b) {
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;
    return a;
}

template <typename T>
Vec3<T> &operator-=(Vec3<T> &a, const Vec3<T> &b) {
    a.x -= b.x;
    a.y -= b.y;
    a.z -= b.z;
    return a;
}

template <typename T>
Vec3<T> &operator*=(Vec3<T> &v, vector_scalar_t scalar) {
    v.x *= scalar;
    v.y *= scalar;
    v.z *= scalar;
    return v;
}

template <typename T>
Vec3<T> &operator/=(Vec3<T> &v, vector_scalar_t scalar) {
    v.x /= scalar;
    v.y /= scalar;
    v.z /= scalar;
    return v;
}

template <typename T>
bool operator==(const Vec3<T> &a, const Vec3<T> &b) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

template <typename T>
bool operator!=(const Vec3<T> &a, const Vec3<T> &b) {
    return !(a == b);
}

template <typename T>
vector_scalar_t dot(const Vec3<T> &a, const Vec3<T> &b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

template <typename T>
vector_scalar_t length_squared(const Vec3<T> &v) {
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

template <typename T>
vector_scalar_t length(const Vec3<T> &v) {
    return sqrt(length_squared(v));
}

template <typename T>
Vec3<T> normalize(const Vec3<T> &v) {
    vector_scalar_t len = length(v);
    return v / len;
}

template <typename T>
vector_scalar_t distance(const Vec3<T> &a, const Vec3<T> &b) {
    return length(a - b);
}

template <typename T>
vector_scalar_t distance_squared(const Vec3<T> &a, const Vec3<T> &b) {
    return length_squared(a - b);
}

} // namespace mia

#endif
