#define ARRLIST_IMPLEMENTATION
#include "arrlist.hpp"
#include "define.hpp"
#include "math.hpp"

#include <unordered_set> // TODO: idk

namespace mia {

#ifndef AABBTREE_IMPLEMENTATION
struct AABBTree_Node {
    AABBTree_Node *parent;
    AABBTree_Node *childs[2];

    AABB2f bound;
    AABB2f *data;
};

struct AABBTree {
    AABBTree_Node *root;
    f32 margin;
};

struct AABBPair {
    AABB2f *first, *second;
};
using AABBPairList = ArrList<AABBPair>;
#endif

// =========================================================

namespace _ {
[[nodiscard]] auto is_node_leaf(const AABBTree_Node &node) -> bool;
void insert_node_helper(AABBTree_Node *node, AABB2f *const aabb, AABBTree *tree, AABBTree_Node **cur_link);
void update_invalid_nodes_helper(ArrList<AABBTree_Node *> *invalid_list, AABBTree_Node *cur);
void all_collide_pair_helper(AABBPairList *list, AABBTree_Node *node0, AABBTree_Node *node1,
                             std::unordered_set<AABBTree_Node *> *checked_childs_set);
void handle_self_collide_pair(AABBTree_Node *node, AABBPairList *list, std::unordered_set<AABBTree_Node *> *checked_childs_set);
} // namespace _

// =========================================================
namespace aabbtree {
void insert(AABBTree *tree, AABB2f *aabb) {
    if (tree->root == nullptr) {
        auto node = new AABBTree_Node{};
        node->bound = {{aabb->min - (Vec2f){tree->margin, tree->margin}}, {aabb->max + (Vec2f){tree->margin, tree->margin}}};
        node->data = aabb;

        tree->root = node;

        return;
    }

    _::insert_node_helper(new AABBTree_Node{}, aabb, tree, &tree->root);
}

void update(AABBTree *tree) {
    if (tree->root == nullptr) return;

    if (_::is_node_leaf(*tree->root)) {
        auto &root = tree->root;
        root->bound = {{root->data->min - (Vec2f){tree->margin, tree->margin}},
                       {root->data->max + (Vec2f){tree->margin, tree->margin}}};

        return;
    }

    ArrList<AABBTree_Node *> invalid_nodes = {};
    _::update_invalid_nodes_helper(&invalid_nodes, tree->root);

    for (auto node : invalid_nodes) {
        cauto parent = node->parent;
        auto sibling = node == parent->childs[0] ? parent->childs[1] : parent->childs[0];
        auto parent_link = parent->parent
                               ? (parent == parent->parent->childs[0] ? &parent->parent->childs[0] : &parent->parent->childs[1])
                               : &tree->root;

        sibling->parent = parent->parent ? parent->parent : nullptr;
        *parent_link = sibling;
        delete parent;

        _::insert_node_helper(node, node->data, tree, &tree->root);
    }

    arrlist_free(&invalid_nodes);
}

[[nodiscard]] auto get_collided_pairs(AABBTree *tree) -> AABBPairList {
    AABBPairList res{};

    if (tree->root == nullptr) return res;
    if (_::is_node_leaf(*tree->root)) return res;

    std::unordered_set<AABBTree_Node *> checked_childs_set{};
    _::all_collide_pair_helper(&res, tree->root->childs[0], tree->root->childs[1], &checked_childs_set);
    return res;
}
} // namespace aabbtree

[[nodiscard]] auto _::is_node_leaf(const AABBTree_Node &node) -> bool {
    return node.childs[0] == nullptr && node.childs[1] == nullptr;
}

void _::insert_node_helper(AABBTree_Node *node, AABB2f *const aabb, AABBTree *tree, AABBTree_Node **cur_link) {
    auto cur = *cur_link;
    if (is_node_leaf(*cur)) {
        node->bound = {{aabb->min - (Vec2f){tree->margin, tree->margin}}, {aabb->max + (Vec2f){tree->margin, tree->margin}}};
        node->data = aabb;

        auto new_parent = new AABBTree_Node{};
        new_parent->bound = aabb_merge(cur->bound, node->bound);

        new_parent->parent = cur->parent;
        new_parent->childs[0] = cur;
        new_parent->childs[1] = node;

        cur->parent = new_parent;
        node->parent = new_parent;

        *cur_link = new_parent;

        return;
    }

    cauto bound_diff_0 = aabb_volume(aabb_merge(cur->childs[0]->bound, *aabb)) - aabb_volume(cur->childs[0]->bound);
    cauto bound_diff_1 = aabb_volume(aabb_merge(cur->childs[1]->bound, *aabb)) - aabb_volume(cur->childs[1]->bound);
    if (bound_diff_0 < bound_diff_1) {
        insert_node_helper(node, aabb, tree, &cur->childs[0]);
    } else {
        insert_node_helper(node, aabb, tree, &cur->childs[1]);
    }
    cur->bound = aabb_merge(cur->childs[0]->bound, cur->childs[1]->bound);
}

void _::update_invalid_nodes_helper(ArrList<AABBTree_Node *> *invalid_list, AABBTree_Node *cur) {
    if (is_node_leaf(*cur)) {
        if (!aabb_contains(cur->bound, *cur->data)) {
            arrlist_append(invalid_list, cur);
        }
    } else {
        update_invalid_nodes_helper(invalid_list, cur->childs[0]);
        update_invalid_nodes_helper(invalid_list, cur->childs[1]);
    }
}

void _::handle_self_collide_pair(AABBTree_Node *node, AABBPairList *list,
                                         std::unordered_set<AABBTree_Node *> *checked_childs_set) {
    if (checked_childs_set->find(node) == checked_childs_set->end()) {
        all_collide_pair_helper(list, node->childs[0], node->childs[1], checked_childs_set);
        checked_childs_set->insert(node);
    }
}

void _::all_collide_pair_helper(AABBPairList *list, AABBTree_Node *node0, AABBTree_Node *node1,
                                        std::unordered_set<AABBTree_Node *> *checked_childs_set) {
    if (is_node_leaf(*node0) && is_node_leaf(*node1)) {
        if (aabb_intersects(*node0->data, *node1->data)) {
            arrlist_append(list, {node0->data, node1->data});
        }
        return;
    }

    if (!aabb_intersects(node0->bound, node1->bound)) {
        if (!is_node_leaf(*node0)) handle_self_collide_pair(node0, list, checked_childs_set);

        if (!is_node_leaf(*node1)) handle_self_collide_pair(node1, list, checked_childs_set);

        return;
    }

    if (is_node_leaf(*node0)) {
        handle_self_collide_pair(node1, list, checked_childs_set);

        all_collide_pair_helper(list, node0, node1->childs[0], checked_childs_set);
        all_collide_pair_helper(list, node0, node1->childs[1], checked_childs_set);

        return;
    }
    if (is_node_leaf(*node1)) {
        handle_self_collide_pair(node0, list, checked_childs_set);

        all_collide_pair_helper(list, node0->childs[0], node1, checked_childs_set);
        all_collide_pair_helper(list, node0->childs[1], node1, checked_childs_set);

        return;
    }

    handle_self_collide_pair(node0, list, checked_childs_set);
    handle_self_collide_pair(node1, list, checked_childs_set);

    all_collide_pair_helper(list, node0->childs[0], node1->childs[0], checked_childs_set);
    all_collide_pair_helper(list, node0->childs[0], node1->childs[1], checked_childs_set);
    all_collide_pair_helper(list, node0->childs[1], node1->childs[0], checked_childs_set);
    all_collide_pair_helper(list, node0->childs[1], node1->childs[1], checked_childs_set);
}

} // namespace mia
