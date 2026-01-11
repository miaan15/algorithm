#ifndef AABBTREE_HPP
#define AABBTREE_HPP

#include "define.hpp"
#include "math.hpp"

namespace mia {

struct AABBTree_Node;

struct AABBTree;

void aabbtree_insert(AABBTree *tree, AABB2f *aabb);
void aabbtree_update(AABBTree *tree);

} // namespace mia

#ifdef AABBTREE_IMPLEMENTATION
#include "aabb_tree.cpp"
#endif

#endif // AABBTREE_HPP
