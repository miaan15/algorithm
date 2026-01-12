#ifndef AABBTREE_HPP
#define AABBTREE_HPP

#define ARRLIST_IMPLEMENTATION
#include "arrlist.hpp"
#include "define.hpp"
#include "math.hpp"

namespace mia {

namespace _private {
struct Node;
struct FitNodeValue;
} // namespace _private

struct AABBTree {
    _private::Node *root;
    f32 margin;
};

struct AABBPair {
    AABB2f *first, *second;
};
using AABBPairList = ArrList<AABBPair>;

struct _private::Node {
    Node *parent;
    Node *childs[2];

    AABB2f bound;
    AABB2f *data;

    bool is_self_check;
};
struct _private::FitNodeValue {
    Node *node;
    Node **link;
    f32 value;
};

namespace aabbtree {
void insert(AABBTree *tree, AABB2f *aabb);
void update(AABBTree *tree);
[[nodiscard]] auto get_collided_pairs(AABBTree *tree) -> AABBPairList;
} // namespace aabbtree

} // namespace mia

#ifdef AABBTREE_IMPLEMENTATION
#include "aabbtree.cpp"
#endif

#endif // AABBTREE_HPP
