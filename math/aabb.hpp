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
Vec2<T> size(const AABB2<T> &box) {
    return box.upper_bound - box.lower_bound;
}

template <typename T>
vector_scalar_t area(const AABB2<T> &box) {
    auto s = size(box);
    return s.x * s.y;
}

template <typename T>
Vec2<T> center(const AABB2<T> &box) {
    return (box.lower_bound + box.upper_bound) / 2;
}

template <typename T>
bool contains(const AABB2<T> &box, const Vec2<T> &point) {
    return point.x >= box.lower_bound.x && point.x <= box.upper_bound.x &&
           point.y >= box.lower_bound.y && point.y <= box.upper_bound.y;
}

template <typename T>
AABB2<T> merge(const AABB2<T> &a, const AABB2<T> &b) {
    return {{std::min(a.lower_bound.x, b.lower_bound.x),
             std::min(a.lower_bound.y, b.lower_bound.y)},
            {std::max(a.upper_bound.x, b.upper_bound.x),
             std::max(a.upper_bound.y, b.upper_bound.y)}};
}

template <typename T>
bool intersects(const AABB2<T> &a, const AABB2<T> &b) {
    return a.lower_bound.x <= b.upper_bound.x &&
           a.upper_bound.x >= b.lower_bound.x &&
           a.lower_bound.y <= b.upper_bound.y &&
           a.upper_bound.y >= b.lower_bound.y;
}

template <typename T>
Vec3<T> size(const AABB3<T> &box) {
    return box.upper_bound - box.lower_bound;
}

template <typename T>
vector_scalar_t volume(const AABB3<T> &box) {
    auto s = size(box);
    return s.x * s.y * s.z;
}

template <typename T>
Vec3<T> center(const AABB3<T> &box) {
    return (box.lower_bound + box.upper_bound) / 2;
}

template <typename T>
bool contains(const AABB3<T> &box, const Vec3<T> &point) {
    return point.x >= box.lower_bound.x && point.x <= box.upper_bound.x &&
           point.y >= box.lower_bound.y && point.y <= box.upper_bound.y &&
           point.z >= box.lower_bound.z && point.z <= box.upper_bound.z;
}

template <typename T>
AABB3<T> merge(const AABB3<T> &a, const AABB3<T> &b) {
    return {{std::min(a.lower_bound.x, b.lower_bound.x),
             std::min(a.lower_bound.y, b.lower_bound.y),
             std::min(a.lower_bound.z, b.lower_bound.z)},
            {std::max(a.upper_bound.x, b.upper_bound.x),
             std::max(a.upper_bound.y, b.upper_bound.y),
             std::max(a.upper_bound.z, b.upper_bound.z)}};
}

template <typename T>
bool intersects(const AABB3<T> &a, const AABB3<T> &b) {
    return a.lower_bound.x <= b.upper_bound.x &&
           a.upper_bound.x >= b.lower_bound.x &&
           a.lower_bound.y <= b.upper_bound.y &&
           a.upper_bound.y >= b.lower_bound.y &&
           a.lower_bound.z <= b.upper_bound.z &&
           a.upper_bound.z >= b.lower_bound.z;
}

} // namespace mia

#endif
