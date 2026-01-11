#ifndef AABB_HPP
#define AABB_HPP

#include <algorithm>

#include "vector.hpp"

namespace mia {

template <typename T>
struct AABB2 {
    Vec2<T> lower_bound, upper_bound;
};
using AABB2f = AABB2<f32>;
using AABB2i = AABB2<i32>;
using AABB2u = AABB2<u32>;

template <typename T>
struct AABB3 {
    Vec3<T> lower_bound, upper_bound;
};
using AABB3f = AABB3<f32>;
using AABB3i = AABB3<i32>;
using AABB3u = AABB3<u32>;

template <typename T>
Vec2<T> aabb_size(AABB2<T> const &box) {
    return box.upper_bound - box.lower_bound;
}

template <typename T>
vector_scalar_t aabb_volume(AABB2<T> const &box) {
    auto s = aabb_size(box);
    return s.x * s.y;
}

template <typename T>
Vec2<T> aabb_center(AABB2<T> const &box) {
    return (box.lower_bound + box.upper_bound) / 2;
}

template <typename T>
bool aabb_contains(AABB2<T> const &box, Vec2<T> const &point) {
    return point.x >= box.lower_bound.x && point.x <= box.upper_bound.x &&
           point.y >= box.lower_bound.y && point.y <= box.upper_bound.y;
}

template <typename T>
bool aabb_contains(AABB2<T> const &a, AABB2<T> const &b) {
    return a.lower_bound.x <= b.lower_bound.x &&
           a.upper_bound.x >= b.upper_bound.x &&
           a.lower_bound.y <= b.lower_bound.y &&
           a.upper_bound.y >= b.upper_bound.y;
}

template <typename T>
AABB2<T> aabb_merge(AABB2<T> const &a, AABB2<T> const &b) {
    return {{std::min(a.lower_bound.x, b.lower_bound.x),
             std::min(a.lower_bound.y, b.lower_bound.y)},
            {std::max(a.upper_bound.x, b.upper_bound.x),
             std::max(a.upper_bound.y, b.upper_bound.y)}};
}

template <typename T>
bool aabb_intersects(AABB2<T> const &a, AABB2<T> const &b) {
    return a.lower_bound.x <= b.upper_bound.x &&
           a.upper_bound.x >= b.lower_bound.x &&
           a.lower_bound.y <= b.upper_bound.y &&
           a.upper_bound.y >= b.lower_bound.y;
}

template <typename T>
Vec3<T> aabb_size(AABB3<T> const &box) {
    return box.upper_bound - box.lower_bound;
}

template <typename T>
vector_scalar_t aabb_volume(AABB3<T> const &box) {
    auto s = aabb_size(box);
    return s.x * s.y * s.z;
}

template <typename T>
Vec3<T> aabb_center(AABB3<T> const &box) {
    return (box.lower_bound + box.upper_bound) / 2;
}

template <typename T>
bool aabb_contains(AABB3<T> const &box, Vec3<T> const &point) {
    return point.x >= box.lower_bound.x && point.x <= box.upper_bound.x &&
           point.y >= box.lower_bound.y && point.y <= box.upper_bound.y &&
           point.z >= box.lower_bound.z && point.z <= box.upper_bound.z;
}

template <typename T>
bool aabb_contains(AABB3<T> const &a, AABB3<T> const &b) {
    return a.lower_bound.x <= b.lower_bound.x &&
           a.upper_bound.x >= b.upper_bound.x &&
           a.lower_bound.y <= b.lower_bound.y &&
           a.upper_bound.y >= b.upper_bound.y &&
           a.lower_bound.z <= b.lower_bound.z &&
           a.upper_bound.z >= b.upper_bound.z;
}

template <typename T>
AABB3<T> aabb_merge(AABB3<T> const &a, AABB3<T> const &b) {
    return {{std::min(a.lower_bound.x, b.lower_bound.x),
             std::min(a.lower_bound.y, b.lower_bound.y),
             std::min(a.lower_bound.z, b.lower_bound.z)},
            {std::max(a.upper_bound.x, b.upper_bound.x),
             std::max(a.upper_bound.y, b.upper_bound.y),
             std::max(a.upper_bound.z, b.upper_bound.z)}};
}

template <typename T>
bool aabb_intersects(AABB3<T> const &a, AABB3<T> const &b) {
    return a.lower_bound.x <= b.upper_bound.x &&
           a.upper_bound.x >= b.lower_bound.x &&
           a.lower_bound.y <= b.upper_bound.y &&
           a.upper_bound.y >= b.lower_bound.y &&
           a.lower_bound.z <= b.upper_bound.z &&
           a.upper_bound.z >= b.lower_bound.z;
}

} // namespace mia

#endif
