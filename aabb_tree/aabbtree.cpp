#include <cstdio>
#define ARRLIST_IMPLEMENTATION
#include "arrlist.hpp"
#include "define.hpp"
#include "math.hpp"

namespace mia {

namespace _private {}
using namespace _private;

#ifndef AABBTREE_IMPLEMENTATION
namespace _private {
struct Node;
struct FitNodeValue;
} // namespace _private

struct AABBTree {
    Node *root;
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
#endif

// =========================================================

namespace _private {
[[nodiscard]] auto is_node_leaf(const Node &node) -> bool;
void find_best_fit_node_insert_helper(FitNodeValue *best, const AABB2f &bound, Node *cur, Node **cur_link, f32 accumulate_delta);
void insert_node_from_old(AABBTree *tree, Node *node);
void update_invalid_nodes_helper(ArrList<Node *> *invalid_list, Node *cur);
void get_collided_pairs_helper(AABBPairList *list, Node *node0, Node *node1);
void uncheck_all_node_flag_helper(Node *node);
void handle_self_collide_pair(Node *node, AABBPairList *list);
} // namespace _private

// =========================================================
namespace aabbtree {
void insert(AABBTree *tree, AABB2f *aabb) {
    if (tree->root == nullptr) {
        auto node = new Node{};
        node->bound = {{aabb->min - (Vec2f){tree->margin, tree->margin}}, {aabb->max + (Vec2f){tree->margin, tree->margin}}};
        node->data = aabb;

        tree->root = node;

        return;
    }

    cauto fat_bound = (AABB2f){{aabb->min - (Vec2f){tree->margin, tree->margin}}, {aabb->max + (Vec2f){tree->margin, tree->margin}}};
    auto best = (FitNodeValue){.node = tree->root, .link = &tree->root, .value = aabb_volume(aabb_merge(fat_bound, tree->root->bound))};
    find_best_fit_node_insert_helper(&best, fat_bound, tree->root, &tree->root, 0);

    auto node = new Node{};
    node->bound = {{aabb->min - (Vec2f){tree->margin, tree->margin}}, {aabb->max + (Vec2f){tree->margin, tree->margin}}};
    node->data = aabb;

    auto new_parent = new Node{};
    new_parent->bound = aabb_merge(best.node->bound, node->bound);

    new_parent->parent = best.node->parent;
    new_parent->childs[0] = best.node;
    new_parent->childs[1] = node;

    best.node->parent = new_parent;
    node->parent = new_parent;

    *best.link = new_parent;
}

void update(AABBTree *tree) {
    if (tree->root == nullptr) return;

    if (is_node_leaf(*tree->root)) {
        auto &root = tree->root;
        root->bound = {{root->data->min - (Vec2f){tree->margin, tree->margin}},
                       {root->data->max + (Vec2f){tree->margin, tree->margin}}};

        return;
    }

    ArrList<Node *> invalid_nodes = {};
    update_invalid_nodes_helper(&invalid_nodes, tree->root);

    for (auto node : invalid_nodes) {
        cauto parent = node->parent;
        auto sibling = node == parent->childs[0] ? parent->childs[1] : parent->childs[0];
        auto parent_link = parent->parent
                               ? (parent == parent->parent->childs[0] ? &parent->parent->childs[0] : &parent->parent->childs[1])
                               : &tree->root;

        sibling->parent = parent->parent ? parent->parent : nullptr;
        *parent_link = sibling;
        delete parent;

        insert_node_from_old(tree, node);
    }

    arrlist_free(&invalid_nodes);
}

[[nodiscard]] auto get_collided_pairs(AABBTree *tree) -> AABBPairList {
    AABBPairList res{};

    if (tree->root == nullptr) return res;
    if (is_node_leaf(*tree->root)) return res;

    uncheck_all_node_flag_helper(tree->root);
    get_collided_pairs_helper(&res, tree->root->childs[0], tree->root->childs[1]);

    return res;
}
} // namespace aabbtree

[[nodiscard]] auto _private::is_node_leaf(const Node &node) -> bool {
    return node.childs[0] == nullptr || node.childs[1] == nullptr;
}

void _private::find_best_fit_node_insert_helper(FitNodeValue *best, const AABB2f &bound, Node *cur, Node **cur_link,
                                                f32 accumulate_delta) {
    cauto cur_value = aabb_volume(aabb_merge(bound, cur->bound)) + accumulate_delta;
    if (cur_value < best->value) {
        best->node = cur;
        best->link = cur_link;
        best->value = cur_value;
    }

    if (is_node_leaf(*cur)) return;

    cauto cur_delta = aabb_volume(aabb_merge(bound, cur->bound)) - aabb_volume(cur->bound);
    if (aabb_volume(bound) + cur_delta + accumulate_delta < best->value) {
        find_best_fit_node_insert_helper(best, bound, cur->childs[0], &cur->childs[0], accumulate_delta + cur_delta);
        find_best_fit_node_insert_helper(best, bound, cur->childs[1], &cur->childs[1], accumulate_delta + cur_delta);
    }
}

void _private::insert_node_from_old(AABBTree *tree, Node *node) {
    cauto &aabb = node->data;
    if (tree->root == nullptr) {
        node->bound = {{aabb->min - (Vec2f){tree->margin, tree->margin}}, {aabb->max + (Vec2f){tree->margin, tree->margin}}};
        node->data = aabb;

        tree->root = node;

        return;
    }

    cauto fat_bound = (AABB2f){{aabb->min - (Vec2f){tree->margin, tree->margin}}, {aabb->max + (Vec2f){tree->margin, tree->margin}}};
    auto best = (FitNodeValue){.node = tree->root, .link = nullptr, .value = aabb_volume(aabb_merge(fat_bound, tree->root->bound))};
    find_best_fit_node_insert_helper(&best, fat_bound, tree->root, nullptr, 0);

    node->bound = {{aabb->min - (Vec2f){tree->margin, tree->margin}}, {aabb->max + (Vec2f){tree->margin, tree->margin}}};
    node->data = aabb;

    auto new_parent = new Node{};
    new_parent->bound = aabb_merge(best.node->bound, node->bound);

    new_parent->parent = best.node->parent;
    new_parent->childs[0] = best.node;
    new_parent->childs[1] = node;

    best.node->parent = new_parent;
    node->parent = new_parent;

    if (best.link != nullptr) {
        *best.link = new_parent;
    }
}

void _private::update_invalid_nodes_helper(ArrList<Node *> *invalid_list, Node *cur) {
    if (is_node_leaf(*cur)) {
        if (!aabb_contains(cur->bound, *cur->data)) {
            arrlist_append(invalid_list, cur);
        }
    } else {
        update_invalid_nodes_helper(invalid_list, cur->childs[0]);
        update_invalid_nodes_helper(invalid_list, cur->childs[1]);
    }
}

void _private::handle_self_collide_pair(Node *node, AABBPairList *list) {
    if (!node->is_self_check) {
        get_collided_pairs_helper(list, node->childs[0], node->childs[1]);
        node->is_self_check = true;
    }
}

void _private::uncheck_all_node_flag_helper(Node *node) {
    node->is_self_check = false;

    if (is_node_leaf(*node)) return;

    uncheck_all_node_flag_helper(node->childs[0]);
    uncheck_all_node_flag_helper(node->childs[1]);
}

void _private::get_collided_pairs_helper(AABBPairList *list, Node *node0, Node *node1) {
    if (is_node_leaf(*node0) && is_node_leaf(*node1)) {
        if (aabb_intersects(*node0->data, *node1->data)) {
            arrlist_append(list, {node0->data, node1->data});
        }
        return;
    }

    if (!aabb_intersects(node0->bound, node1->bound)) {
        if (!is_node_leaf(*node0)) handle_self_collide_pair(node0, list);

        if (!is_node_leaf(*node1)) handle_self_collide_pair(node1, list);

        return;
    }

    if (is_node_leaf(*node0)) {
        handle_self_collide_pair(node1, list);

        get_collided_pairs_helper(list, node0, node1->childs[0]);
        get_collided_pairs_helper(list, node0, node1->childs[1]);

        return;
    }
    if (is_node_leaf(*node1)) {
        handle_self_collide_pair(node0, list);

        get_collided_pairs_helper(list, node0->childs[0], node1);
        get_collided_pairs_helper(list, node0->childs[1], node1);

        return;
    }

    handle_self_collide_pair(node0, list);
    handle_self_collide_pair(node1, list);

    get_collided_pairs_helper(list, node0->childs[0], node1->childs[0]);
    get_collided_pairs_helper(list, node0->childs[0], node1->childs[1]);
    get_collided_pairs_helper(list, node0->childs[1], node1->childs[0]);
    get_collided_pairs_helper(list, node0->childs[1], node1->childs[1]);
}

} // namespace mia
