#ifndef AABB_HPP
#define AABB_HPP

#include <algorithm>

#include "vector.hpp"

namespace mia {

template <typename T>
struct AABB2 {
    Vec2<T> min, max;
};
using AABB2f = AABB2<f32>;
using AABB2i = AABB2<i32>;
using AABB2u = AABB2<u32>;

template <typename T>
struct AABB3 {
    Vec3<T> min, max;
};
using AABB3f = AABB3<f32>;
using AABB3i = AABB3<i32>;
using AABB3u = AABB3<u32>;

namespace aabb {
template <typename T>
Vec2<T> size(AABB2<T> const &box) {
    return box.max - box.min;
}

template <typename T>
vector_scalar_t volume(AABB2<T> const &box) {
    auto s = size(box);
    return s.x * s.y;
}

template <typename T>
Vec2<T> center(AABB2<T> const &box) {
    return (box.min + box.max) / 2;
}

template <typename T>
bool contains(AABB2<T> const &box, Vec2<T> const &point) {
    return point.x >= box.min.x && point.x <= box.max.x &&
           point.y >= box.min.y && point.y <= box.max.y;
}

template <typename T>
bool contains(AABB2<T> const &a, AABB2<T> const &b) {
    return a.min.x <= b.min.x && a.max.x >= b.max.x && a.min.y <= b.min.y &&
           a.max.y >= b.max.y;
}

template <typename T>
AABB2<T> merge(AABB2<T> const &a, AABB2<T> const &b) {
    return {{std::min(a.min.x, b.min.x), std::min(a.min.y, b.min.y)},
            {std::max(a.max.x, b.max.x), std::max(a.max.y, b.max.y)}};
}

template <typename T>
bool intersects(AABB2<T> const &a, AABB2<T> const &b) {
    return a.min.x <= b.max.x && a.max.x >= b.min.x && a.min.y <= b.max.y &&
           a.max.y >= b.min.y;
}

template <typename T>
Vec3<T> size(AABB3<T> const &box) {
    return box.max - box.min;
}

template <typename T>
vector_scalar_t volume(AABB3<T> const &box) {
    auto s = size(box);
    return s.x * s.y * s.z;
}

template <typename T>
Vec3<T> center(AABB3<T> const &box) {
    return (box.min + box.max) / 2;
}

template <typename T>
bool contains(AABB3<T> const &box, Vec3<T> const &point) {
    return point.x >= box.min.x && point.x <= box.max.x &&
           point.y >= box.min.y && point.y <= box.max.y &&
           point.z >= box.min.z && point.z <= box.max.z;
}

template <typename T>
bool contains(AABB3<T> const &a, AABB3<T> const &b) {
    return a.min.x <= b.min.x && a.max.x >= b.max.x && a.min.y <= b.min.y &&
           a.max.y >= b.max.y && a.min.z <= b.min.z && a.max.z >= b.max.z;
}

template <typename T>
AABB3<T> merge(AABB3<T> const &a, AABB3<T> const &b) {
    return {{std::min(a.min.x, b.min.x), std::min(a.min.y, b.min.y),
             std::min(a.min.z, b.min.z)},
            {std::max(a.max.x, b.max.x), std::max(a.max.y, b.max.y),
             std::max(a.max.z, b.max.z)}};
}

template <typename T>
bool intersects(AABB3<T> const &a, AABB3<T> const &b) {
    return a.min.x <= b.max.x && a.max.x >= b.min.x && a.min.y <= b.max.y &&
           a.max.y >= b.min.y && a.min.z <= b.max.z && a.max.z >= b.min.z;
}
} // namespace aabb

} // namespace mia

#endif
