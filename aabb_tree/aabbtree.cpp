// TODO: Implement tree rotate for futher improvement

#include "aabbtree.hpp"

namespace mia {

#ifndef AABBTREE_HPP
struct AABBTreeNode {
    AABBTreeNode *parent;
    AABBTreeNode *childs[2];

    AABB2f bound;
    usize index;

    bool is_self_check;
};
struct AABBTree {
    AABBTreeNode *root;
    f32 margin;

    Pool<AABB2f> data;
};

struct AABBPair {
    AABB2f *first, *second;
};
using AABBPairList = ArrList<AABBPair>;
#endif

struct FitNodeValue {
    AABBTreeNode *node;
    AABBTreeNode **link;
    f32 value;
};

[[nodiscard]] auto _get_node(AABBTree *tree, usize index) -> AABB2f *;
[[nodiscard]] auto _is_node_leaf(const AABBTreeNode &node) -> bool;
void _find_best_fit_node_insert_helper(FitNodeValue *best, const AABB2f &bound, AABBTreeNode *cur, AABBTreeNode **cur_link,
                                       f32 accumulate_delta);
void _insert_node_from_old(AABBTree *tree, AABBTreeNode *node);
void _update_invalid_nodes_helper(AABBTree *tree, ArrList<AABBTreeNode *> *invalid_list, AABBTreeNode *cur);
void _get_collided_pairs_helper(AABBTree *tree, AABBPairList *list, AABBTreeNode *node0, AABBTreeNode *node1);
void _uncheck_all_node_flag_helper(AABBTreeNode *node);
void _handle_self_collide_pair(AABBTree *tree, AABBTreeNode *node, AABBPairList *list);

namespace aabbtree {
void insert(AABBTree *tree, const AABB2f &aabb) {
    if (tree->root == nullptr) {
        auto node = new AABBTreeNode{};

        node->bound = {{aabb.min - (Vec2f){tree->margin, tree->margin}}, {aabb.max + (Vec2f){tree->margin, tree->margin}}};
        node->index = pool::insert(&tree->data, aabb);

        tree->root = node;

        return;
    }

    cauto fat_bound = (AABB2f){{aabb.min - (Vec2f){tree->margin, tree->margin}}, {aabb.max + (Vec2f){tree->margin, tree->margin}}};
    auto best =
        (FitNodeValue){.node = tree->root, .link = &tree->root, .value = aabb::volume(aabb::merge(fat_bound, tree->root->bound))};
    _find_best_fit_node_insert_helper(&best, fat_bound, tree->root, &tree->root, 0);

    auto node = new AABBTreeNode{};
    node->bound = {{aabb.min - (Vec2f){tree->margin, tree->margin}}, {aabb.max + (Vec2f){tree->margin, tree->margin}}};
    node->index = pool::insert(&tree->data, aabb);

    auto new_parent = new AABBTreeNode{};
    new_parent->bound = aabb::merge(best.node->bound, node->bound);

    new_parent->parent = best.node->parent;
    new_parent->childs[0] = best.node;
    new_parent->childs[1] = node;

    best.node->parent = new_parent;
    node->parent = new_parent;

    *best.link = new_parent;
}

void update(AABBTree *tree) {
    if (tree->root == nullptr) return;

    if (_is_node_leaf(*tree->root)) {
        auto &root = tree->root;
        root->bound = {{tree->data[root->index].min - (Vec2f){tree->margin, tree->margin}},
                       {tree->data[root->index].max + (Vec2f){tree->margin, tree->margin}}};

        return;
    }

    ArrList<AABBTreeNode *> invalid_nodes = {};
    _update_invalid_nodes_helper(tree, &invalid_nodes, tree->root);

    for (auto node : invalid_nodes) {
        cauto parent = node->parent;
        auto sibling = node == parent->childs[0] ? parent->childs[1] : parent->childs[0];
        auto parent_link = parent->parent
                               ? (parent == parent->parent->childs[0] ? &parent->parent->childs[0] : &parent->parent->childs[1])
                               : &tree->root;

        sibling->parent = parent->parent ? parent->parent : nullptr;
        *parent_link = sibling;
        delete parent;

        _insert_node_from_old(tree, node);
    }

    arrlist::free(&invalid_nodes);
}

[[nodiscard]] auto get_collided_pairs(AABBTree *tree) -> AABBPairList {
    AABBPairList res{};

    if (tree->root == nullptr) return res;
    if (_is_node_leaf(*tree->root)) return res;

    _uncheck_all_node_flag_helper(tree->root);
    _get_collided_pairs_helper(tree, &res, tree->root->childs[0], tree->root->childs[1]);

    return res;
}
} // namespace aabbtree

[[nodiscard]] auto _get_node(AABBTree *tree, usize index) -> AABB2f * {
    return &tree->data[index];
}

[[nodiscard]] auto _is_node_leaf(const AABBTreeNode &node) -> bool {
    return node.childs[0] == nullptr || node.childs[1] == nullptr;
}

void _find_best_fit_node_insert_helper(FitNodeValue *best, const AABB2f &bound, AABBTreeNode *cur, AABBTreeNode **cur_link,
                                       f32 accumulate_delta) {
    cauto cur_value = aabb::volume(aabb::merge(bound, cur->bound)) + accumulate_delta;
    if (cur_value < best->value) {
        best->node = cur;
        best->link = cur_link;
        best->value = cur_value;
    }

    if (_is_node_leaf(*cur)) return;

    cauto cur_delta = aabb::volume(aabb::merge(bound, cur->bound)) - aabb::volume(cur->bound);
    if (aabb::volume(bound) + cur_delta + accumulate_delta < best->value) {
        _find_best_fit_node_insert_helper(best, bound, cur->childs[0], &cur->childs[0], accumulate_delta + cur_delta);
        _find_best_fit_node_insert_helper(best, bound, cur->childs[1], &cur->childs[1], accumulate_delta + cur_delta);
    }
}

void _insert_node_from_old(AABBTree *tree, AABBTreeNode *node) {
    cauto &aabb = tree->data[node->index];
    if (tree->root == nullptr) {
        node->bound = {{aabb.min - (Vec2f){tree->margin, tree->margin}}, {aabb.max + (Vec2f){tree->margin, tree->margin}}};

        tree->root = node;

        return;
    }

    cauto fat_bound = (AABB2f){{aabb.min - (Vec2f){tree->margin, tree->margin}}, {aabb.max + (Vec2f){tree->margin, tree->margin}}};
    auto best = (FitNodeValue){.node = tree->root, .link = nullptr, .value = aabb::volume(aabb::merge(fat_bound, tree->root->bound))};
    _find_best_fit_node_insert_helper(&best, fat_bound, tree->root, nullptr, 0);

    node->bound = {{aabb.min - (Vec2f){tree->margin, tree->margin}}, {aabb.max + (Vec2f){tree->margin, tree->margin}}};

    auto new_parent = new AABBTreeNode{};
    new_parent->bound = aabb::merge(best.node->bound, node->bound);

    new_parent->parent = best.node->parent;
    new_parent->childs[0] = best.node;
    new_parent->childs[1] = node;

    best.node->parent = new_parent;
    node->parent = new_parent;

    if (best.link != nullptr) {
        *best.link = new_parent;
    }
}

void _update_invalid_nodes_helper(AABBTree *tree, ArrList<AABBTreeNode *> *invalid_list, AABBTreeNode *cur) {
    if (_is_node_leaf(*cur)) {
        if (!aabb::contains(cur->bound, tree->data[cur->index])) {
            arrlist::append(invalid_list, cur);
        }
    } else {
        _update_invalid_nodes_helper(tree, invalid_list, cur->childs[0]);
        _update_invalid_nodes_helper(tree, invalid_list, cur->childs[1]);
    }
}

void _handle_self_collide_pair(AABBTree *tree, AABBTreeNode *node, AABBPairList *list) {
    if (!node->is_self_check) {
        _get_collided_pairs_helper(tree, list, node->childs[0], node->childs[1]);
        node->is_self_check = true;
    }
}

void _uncheck_all_node_flag_helper(AABBTreeNode *node) {
    node->is_self_check = false;

    if (_is_node_leaf(*node)) return;

    _uncheck_all_node_flag_helper(node->childs[0]);
    _uncheck_all_node_flag_helper(node->childs[1]);
}

void _get_collided_pairs_helper(AABBTree *tree, AABBPairList *list, AABBTreeNode *node0, AABBTreeNode *node1) {
    if (_is_node_leaf(*node0) && _is_node_leaf(*node1)) {
        if (aabb::intersects(tree->data[node0->index], tree->data[node1->index])) {
            arrlist::append(list, {&tree->data[node0->index], &tree->data[node1->index]});
        }
        return;
    }

    if (!aabb::intersects(node0->bound, node1->bound)) {
        if (!_is_node_leaf(*node0)) _handle_self_collide_pair(tree, node0, list);

        if (!_is_node_leaf(*node1)) _handle_self_collide_pair(tree, node1, list);

        return;
    }

    if (_is_node_leaf(*node0)) {
        _handle_self_collide_pair(tree, node1, list);

        _get_collided_pairs_helper(tree, list, node0, node1->childs[0]);
        _get_collided_pairs_helper(tree, list, node0, node1->childs[1]);

        return;
    }
    if (_is_node_leaf(*node1)) {
        _handle_self_collide_pair(tree, node0, list);

        _get_collided_pairs_helper(tree, list, node0->childs[0], node1);
        _get_collided_pairs_helper(tree, list, node0->childs[1], node1);

        return;
    }

    _handle_self_collide_pair(tree, node0, list);
    _handle_self_collide_pair(tree, node1, list);

    _get_collided_pairs_helper(tree, list, node0->childs[0], node1->childs[0]);
    _get_collided_pairs_helper(tree, list, node0->childs[0], node1->childs[1]);
    _get_collided_pairs_helper(tree, list, node0->childs[1], node1->childs[0]);
    _get_collided_pairs_helper(tree, list, node0->childs[1], node1->childs[1]);
}

} // namespace mia
