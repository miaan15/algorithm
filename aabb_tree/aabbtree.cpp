// TODO: Implement tree rotate for futher improvement

#include "aabbtree.hpp"

#include "arrlist.hpp"
#include "define.hpp"
#include "math.hpp"
#include "pool.hpp"

namespace mia {

#ifndef AABBTREE_HPP
struct AABBTreeNode {
    AABBTreeNode *parent;
    AABBTreeNode *childs[2];

    AABB2f bound;
    AABB2f *data;

    bool is_self_check;
};
struct AABBTree {
    AABBTreeNode *root;
    f32 margin;
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

[[nodiscard]] auto _is_node_leaf(const AABBTreeNode &node) -> bool;
void _find_best_fit_node_insert_helper(FitNodeValue *best, const AABB2f &bound, AABBTreeNode *cur, AABBTreeNode **cur_link,
                                       f32 accumulate_delta);
void _insert_node_from_old(AABBTree *tree, AABBTreeNode *node);
void _update_invalid_nodes_helper(ArrList<AABBTreeNode *> *invalid_list, AABBTreeNode *cur);
void _get_collided_pairs_helper(AABBPairList *list, AABBTreeNode *node0, AABBTreeNode *node1);
void _uncheck_all_node_flag_helper(AABBTreeNode *node);
void _handle_self_collide_pair(AABBTreeNode *node, AABBPairList *list);
auto _handle_remove_helper(AABB2f *aabb, AABBTreeNode *cur, AABBTreeNode **cur_link) -> bool;
void _handle_query_point_helper(ArrList<AABB2f *> *list, const Vec2f &point, AABBTreeNode *cur);
void _handle_query_aabb_helper(ArrList<AABB2f *> *list, const AABB2f &aabb, AABBTreeNode *cur);

namespace aabbtree {
void insert(AABBTree *tree, AABB2f *aabb) {
    if (tree->root == nullptr) {
        auto node = new AABBTreeNode{};
        node->bound = {{aabb->min - (Vec2f){tree->margin, tree->margin}}, {aabb->max + (Vec2f){tree->margin, tree->margin}}};
        node->data = aabb;

        tree->root = node;

        return;
    }

    cauto fat_bound = (AABB2f){{aabb->min - (Vec2f){tree->margin, tree->margin}}, {aabb->max + (Vec2f){tree->margin, tree->margin}}};
    auto best =
        (FitNodeValue){.node = tree->root, .link = &tree->root, .value = aabb::volume(aabb::merge(fat_bound, tree->root->bound))};
    _find_best_fit_node_insert_helper(&best, fat_bound, tree->root, &tree->root, 0);

    auto node = new AABBTreeNode{};
    node->bound = {{aabb->min - (Vec2f){tree->margin, tree->margin}}, {aabb->max + (Vec2f){tree->margin, tree->margin}}};
    node->data = aabb;

    auto new_parent = new AABBTreeNode{};
    new_parent->bound = aabb::merge(best.node->bound, node->bound);

    new_parent->parent = best.node->parent;
    new_parent->childs[0] = best.node;
    new_parent->childs[1] = node;

    best.node->parent = new_parent;
    node->parent = new_parent;

    *best.link = new_parent;

    auto cur_update_node = new_parent->parent;
    while (cur_update_node != nullptr) {
        cur_update_node->bound = aabb::merge(cur_update_node->childs[0]->bound, cur_update_node->childs[1]->bound);
        cur_update_node = cur_update_node->parent;
    }
}

auto remove(AABBTree *tree, AABB2f *aabb) -> bool {
    if (tree->root == nullptr) return false;
    if (_is_node_leaf(*tree->root)) {
        if (aabb == tree->root->data) {
            tree->root = nullptr;
            free(tree->root);
            return true;
        }
        return false;
    }
    return _handle_remove_helper(aabb, tree->root, &tree->root);
}

void update(AABBTree *tree) {
    if (tree->root == nullptr) return;

    if (_is_node_leaf(*tree->root)) {
        auto &root = tree->root;
        root->bound = {{root->data->min - (Vec2f){tree->margin, tree->margin}},
                       {root->data->max + (Vec2f){tree->margin, tree->margin}}};

        return;
    }

    ArrList<AABBTreeNode *> invalid_nodes = {};
    _update_invalid_nodes_helper(&invalid_nodes, tree->root);

    for (auto node : invalid_nodes) {
        cauto parent = node->parent;
        auto sibling = node == parent->childs[0] ? parent->childs[1] : parent->childs[0];
        auto parent_link = parent->parent
                               ? (parent == parent->parent->childs[0] ? &parent->parent->childs[0] : &parent->parent->childs[1])
                               : &tree->root;

        sibling->parent = parent->parent ? parent->parent : nullptr;
        *parent_link = sibling;
        free(parent);

        _insert_node_from_old(tree, node);
    }

    arrlist::free(&invalid_nodes);
}

[[nodiscard]] auto query_point(AABBTree *tree, const Vec2f &point) -> ArrList<AABB2f *> {
    ArrList<AABB2f *> res{};
    if (tree->root == nullptr) return res;
    _handle_query_point_helper(&res, point, tree->root);
    return res;
}

[[nodiscard]] auto query_aabb(AABBTree *tree, const AABB2f &aabb) -> ArrList<AABB2f *> {
    ArrList<AABB2f *> res{};
    if (tree->root == nullptr) return res;
    _handle_query_aabb_helper(&res, aabb, tree->root);
    return res;
}

[[nodiscard]] auto get_collided_pairs(AABBTree *tree) -> AABBPairList {
    AABBPairList res{};

    if (tree->root == nullptr) return res;
    if (_is_node_leaf(*tree->root)) return res;

    _uncheck_all_node_flag_helper(tree->root);
    _get_collided_pairs_helper(&res, tree->root->childs[0], tree->root->childs[1]);

    return res;
}
} // namespace aabbtree

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
    cauto aabb = node->data;
    if (tree->root == nullptr) {
        node->bound = {{aabb->min - (Vec2f){tree->margin, tree->margin}}, {aabb->max + (Vec2f){tree->margin, tree->margin}}};

        tree->root = node;

        return;
    }

    cauto fat_bound = (AABB2f){{aabb->min - (Vec2f){tree->margin, tree->margin}}, {aabb->max + (Vec2f){tree->margin, tree->margin}}};
    auto best =
        (FitNodeValue){.node = tree->root, .link = &tree->root, .value = aabb::volume(aabb::merge(fat_bound, tree->root->bound))};
    _find_best_fit_node_insert_helper(&best, fat_bound, tree->root, &tree->root, 0);

    node->bound = fat_bound;

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

    auto cur_update_node = new_parent->parent;
    while (cur_update_node != nullptr) {
        cur_update_node->bound = aabb::merge(cur_update_node->childs[0]->bound, cur_update_node->childs[1]->bound);
        cur_update_node = cur_update_node->parent;
    }
}

void _update_invalid_nodes_helper(ArrList<AABBTreeNode *> *invalid_list, AABBTreeNode *cur) {
    if (_is_node_leaf(*cur)) {
        if (!aabb::contains(cur->bound, *cur->data)) {
            arrlist::append(invalid_list, cur);
        }
    } else {
        _update_invalid_nodes_helper(invalid_list, cur->childs[0]);
        _update_invalid_nodes_helper(invalid_list, cur->childs[1]);
    }
}

void _handle_self_collide_pair(AABBTreeNode *node, AABBPairList *list) {
    if (!node->is_self_check) {
        _get_collided_pairs_helper(list, node->childs[0], node->childs[1]);
        node->is_self_check = true;
    }
}

void _uncheck_all_node_flag_helper(AABBTreeNode *node) {
    node->is_self_check = false;

    if (_is_node_leaf(*node)) return;

    _uncheck_all_node_flag_helper(node->childs[0]);
    _uncheck_all_node_flag_helper(node->childs[1]);
}

void _get_collided_pairs_helper(AABBPairList *list, AABBTreeNode *node0, AABBTreeNode *node1) {
    if (_is_node_leaf(*node0) && _is_node_leaf(*node1)) {
        if (aabb::intersects(*node0->data, *node1->data)) {
            arrlist::append(list, {node0->data, node1->data});
        }
        return;
    }

    if (!aabb::intersects(node0->bound, node1->bound)) {
        if (!_is_node_leaf(*node0)) _handle_self_collide_pair(node0, list);

        if (!_is_node_leaf(*node1)) _handle_self_collide_pair(node1, list);

        return;
    }

    if (_is_node_leaf(*node0)) {
        _handle_self_collide_pair(node1, list);

        _get_collided_pairs_helper(list, node0, node1->childs[0]);
        _get_collided_pairs_helper(list, node0, node1->childs[1]);

        return;
    }
    if (_is_node_leaf(*node1)) {
        _handle_self_collide_pair(node0, list);

        _get_collided_pairs_helper(list, node0->childs[0], node1);
        _get_collided_pairs_helper(list, node0->childs[1], node1);

        return;
    }

    _handle_self_collide_pair(node0, list);
    _handle_self_collide_pair(node1, list);

    _get_collided_pairs_helper(list, node0->childs[0], node1->childs[0]);
    _get_collided_pairs_helper(list, node0->childs[0], node1->childs[1]);
    _get_collided_pairs_helper(list, node0->childs[1], node1->childs[0]);
    _get_collided_pairs_helper(list, node0->childs[1], node1->childs[1]);
}

auto _handle_remove_helper(AABB2f *aabb, AABBTreeNode *cur, AABBTreeNode **cur_link) -> bool {
    bool res = false;
    if (!_is_node_leaf(*cur->childs[0])) {
        if (aabb::contains(cur->childs[0]->bound, *aabb)) {
            res |= _handle_remove_helper(aabb, cur->childs[0], &cur->childs[0]);
        }
    } else {
        if (cur->childs[0]->data == aabb) {
            *cur_link = cur->childs[1];
            cur->childs[1]->parent = cur->parent;
            free(cur->childs[0]);
            free(cur);
            return true;
        }
    }

    if (!_is_node_leaf(*cur->childs[1])) {
        if (aabb::contains(cur->childs[1]->bound, *aabb)) {
            res |= _handle_remove_helper(aabb, cur->childs[1], &cur->childs[1]);
        }
    } else {
        if (cur->childs[1]->data == aabb) {
            *cur_link = cur->childs[0];
            cur->childs[0]->parent = cur->parent;
            free(cur->childs[1]);
            free(cur);
            return true;
        }
    }

    if (res) {
        cur->bound = aabb::merge(cur->childs[0]->bound, cur->childs[1]->bound);
    }

    return res;
}

void _handle_query_point_helper(ArrList<AABB2f *> *list, const Vec2f &point, AABBTreeNode *cur) {
    if (!aabb::contains(cur->bound, point)) return;
    if (_is_node_leaf(*cur)) {
        if (aabb::contains(*cur->data, point)) {
            arrlist::append(list, cur->data);
        }
        return;
    }
    _handle_query_point_helper(list, point, cur->childs[0]);
    _handle_query_point_helper(list, point, cur->childs[1]);
}

void _handle_query_aabb_helper(ArrList<AABB2f *> *list, const AABB2f &aabb, AABBTreeNode *cur) {
    if (!aabb::intersects(cur->bound, aabb)) return;
    if (_is_node_leaf(*cur)) {
        if (aabb::intersects(*cur->data, aabb)) {
            arrlist::append(list, cur->data);
        }
        return;
    }
    _handle_query_aabb_helper(list, aabb, cur->childs[0]);
    _handle_query_aabb_helper(list, aabb, cur->childs[1]);
}


} // namespace mia
