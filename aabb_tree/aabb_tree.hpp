#ifndef AABBTREE_HPP
#define AABBTREE_HPP

#define ARRLIST_IMPLEMENTATION
#include "arrlist.hpp"
#include "define.hpp"
#include "math.hpp"

namespace mia {

struct AABBTree_Node;

struct AABBTree;

struct AABBPair {
    AABB2f *first, *second;
};
using AABBPairList = ArrList<AABBPair>;

void aabbtree_insert(AABBTree *tree, AABB2f *aabb);
void aabbtree_update(AABBTree *tree);
AABBPairList aabbtree_all_collide_pair(AABBTree *tree);

} // namespace mia

#ifdef AABBTREE_IMPLEMENTATION
#include "aabb_tree.cpp"
#endif

#endif // AABBTREE_HPP
