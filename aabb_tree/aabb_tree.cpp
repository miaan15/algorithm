#define ARRLIST_IMPLEMENTATION
#include "arrlist.hpp"
#include "define.hpp"
#include "math.hpp"

#include <unordered_set> // TODO: idk

namespace mia {

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

#ifndef AABBTREE_IMPLEMENTATION
struct AABBPair {
    AABB2f *first, *second;
};
using AABBPairList = ArrList<AABBPair>;
#endif

bool _is_node_leaf(AABBTree_Node const &node);
void _aabbtree_insert_node_helper(AABBTree_Node *node, AABB2f *const aabb,
                                  AABBTree *tree, AABBTree_Node **cur_link);
void _update_invalid_nodes_helper(ArrList<AABBTree_Node *> *invalid_list,
                                  AABBTree_Node *cur);
void _aabbtree_all_collide_pair_helper(
    AABBPairList *list, AABBTree_Node *node0, AABBTree_Node *node1,
    std::unordered_set<AABBTree_Node *> *checked_childs_set);

void aabbtree_insert(AABBTree *tree, AABB2f *aabb) {
    if (tree->root == nullptr) {
        auto node = new AABBTree_Node{};
        node->bound = {{aabb->min - (Vec2f){tree->margin, tree->margin}},
                       {aabb->max + (Vec2f){tree->margin, tree->margin}}};
        node->data = aabb;

        tree->root = node;

        return;
    }

    _aabbtree_insert_node_helper(new AABBTree_Node{}, aabb, tree, &tree->root);
}

void aabbtree_update(AABBTree *tree) {
    if (tree->root == nullptr)
        return;

    if (_is_node_leaf(*tree->root)) {
        auto &root = tree->root;
        root->bound = {{root->data->min - (Vec2f){tree->margin, tree->margin}},
                       {root->data->max + (Vec2f){tree->margin, tree->margin}}};

        return;
    }

    ArrList<AABBTree_Node *> invalid_nodes = {};
    _update_invalid_nodes_helper(&invalid_nodes, tree->root);

    for (auto node : invalid_nodes) {
        cauto parent = node->parent;
        auto sibling =
            node == parent->childs[0] ? parent->childs[1] : parent->childs[0];
        auto parent_link = parent->parent ? (parent == parent->parent->childs[0]
                                                 ? &parent->parent->childs[0]
                                                 : &parent->parent->childs[1])
                                          : &tree->root;

        sibling->parent = parent->parent ? parent->parent : nullptr;
        *parent_link = sibling;
        delete parent;

        _aabbtree_insert_node_helper(node, node->data, tree, &tree->root);
    }

    arrlist_free(&invalid_nodes);
}

AABBPairList aabbtree_all_collide_pair(AABBTree *tree) {
    AABBPairList res{};

    if (tree->root == nullptr)
        return res;
    if (_is_node_leaf(*tree->root))
        return res;

    std::unordered_set<AABBTree_Node *> checked_childs_set{};
    _aabbtree_all_collide_pair_helper(&res, tree->root->childs[0],
                                      tree->root->childs[1],
                                      &checked_childs_set);
    return res;
}

void _aabbtree_handle_self_collide_pair(
    AABBTree_Node *node, AABBPairList *list,
    std::unordered_set<AABBTree_Node *> *checked_childs_set) {
    if (checked_childs_set->find(node) == checked_childs_set->end()) {
        _aabbtree_all_collide_pair_helper(list, node->childs[0],
                                          node->childs[1], checked_childs_set);
        checked_childs_set->insert(node);
    }
}
void _aabbtree_all_collide_pair_helper(
    AABBPairList *list, AABBTree_Node *node0, AABBTree_Node *node1,
    std::unordered_set<AABBTree_Node *> *checked_childs_set) {
    if (_is_node_leaf(*node0) && _is_node_leaf(*node1)) {
        if (aabb_intersects(*node0->data, *node1->data)) {
            arrlist_append(list, {node0->data, node1->data});
        }
        return;
    }

    if (!aabb_intersects(node0->bound, node1->bound)) {
        if (!_is_node_leaf(*node0))
            _aabbtree_handle_self_collide_pair(node0, list, checked_childs_set);

        if (!_is_node_leaf(*node1))
            _aabbtree_handle_self_collide_pair(node1, list, checked_childs_set);

        return;
    }

    if (_is_node_leaf(*node0)) {
        _aabbtree_handle_self_collide_pair(node1, list, checked_childs_set);

        _aabbtree_all_collide_pair_helper(list, node0, node1->childs[0],
                                          checked_childs_set);
        _aabbtree_all_collide_pair_helper(list, node0, node1->childs[1],
                                          checked_childs_set);

        return;
    }
    if (_is_node_leaf(*node1)) {
        _aabbtree_handle_self_collide_pair(node0, list, checked_childs_set);

        _aabbtree_all_collide_pair_helper(list, node0->childs[0], node1,
                                          checked_childs_set);
        _aabbtree_all_collide_pair_helper(list, node0->childs[1], node1,
                                          checked_childs_set);

        return;
    }

    _aabbtree_handle_self_collide_pair(node0, list, checked_childs_set);
    _aabbtree_handle_self_collide_pair(node1, list, checked_childs_set);

    _aabbtree_all_collide_pair_helper(list, node0->childs[0], node1->childs[0],
                                      checked_childs_set);
    _aabbtree_all_collide_pair_helper(list, node0->childs[0], node1->childs[1],
                                      checked_childs_set);
    _aabbtree_all_collide_pair_helper(list, node0->childs[1], node1->childs[0],
                                      checked_childs_set);
    _aabbtree_all_collide_pair_helper(list, node0->childs[1], node1->childs[1],
                                      checked_childs_set);
}

bool _is_node_leaf(AABBTree_Node const &node) {
    return node.childs[0] == nullptr && node.childs[1] == nullptr;
}

void _aabbtree_insert_node_helper(AABBTree_Node *node, AABB2f *const aabb,
                                  AABBTree *tree, AABBTree_Node **cur_link) {
    auto cur = *cur_link;
    if (_is_node_leaf(*cur)) {
        node->bound = {{aabb->min - (Vec2f){tree->margin, tree->margin}},
                       {aabb->max + (Vec2f){tree->margin, tree->margin}}};
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

    cauto bound_diff_0 = aabb_volume(aabb_merge(cur->childs[0]->bound, *aabb)) -
                         aabb_volume(cur->childs[0]->bound);
    cauto bound_diff_1 = aabb_volume(aabb_merge(cur->childs[1]->bound, *aabb)) -
                         aabb_volume(cur->childs[1]->bound);
    if (bound_diff_0 < bound_diff_1) {
        _aabbtree_insert_node_helper(node, aabb, tree, &cur->childs[0]);
    } else {
        _aabbtree_insert_node_helper(node, aabb, tree, &cur->childs[1]);
    }
    cur->bound = aabb_merge(cur->childs[0]->bound, cur->childs[1]->bound);
}

void _update_invalid_nodes_helper(ArrList<AABBTree_Node *> *invalid_list,
                                  AABBTree_Node *cur) {
    if (_is_node_leaf(*cur)) {
        if (!aabb_contains(cur->bound, *cur->data)) {
            arrlist_append(invalid_list, cur);
        }
    } else {
        _update_invalid_nodes_helper(invalid_list, cur->childs[0]);
        _update_invalid_nodes_helper(invalid_list, cur->childs[1]);
    }
}

} // namespace mia
