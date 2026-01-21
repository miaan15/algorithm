#include "aabbtree.h"

#include <cglm/box.h>
#include <cglm/struct.h>
#include <cglm/types.h>
#include <cglm/vec3.h>

#include "../arrlist/arrlist.h"
#include "../list/list.h"

DEFINE_ARRLIST(uintptr_t);

float _aabb_volume(vec3 aabb[2]) {
    vec3 size;
    glm_vec3_sub(aabb[1], aabb[0], size);
    return size[0] * size[1] * size[2];
}

bool _is_node_leaf(const _AABBTree_Node *const node) {
    return node->childs[0] == nullptr || node->childs[1] == nullptr;
}

void _find_best_fit_node_helper(_AABBTree_FitNodeValue *best, AABB bounds, float accumulate_delta, _AABBTree_Node *cur,
                                _AABBTree_Node **cur_link) {
    AABB mergeBounds;
    glm_aabb_merge(bounds, cur->bounds, mergeBounds);
    float cur_value = _aabb_volume(mergeBounds) + accumulate_delta;
    if (cur_value < best->value) {
        best->node = cur;
        best->link = cur_link;
        best->value = cur_value;
    }

    if (_is_node_leaf(cur)) return;

    float cur_delta = _aabb_volume(mergeBounds) - _aabb_volume(cur->bounds);
    if (_aabb_volume(bounds) + cur_delta + accumulate_delta < best->value) {
        _find_best_fit_node_helper(best, bounds, accumulate_delta + cur_delta, cur->childs[0], &cur->childs[0]);
        _find_best_fit_node_helper(best, bounds, accumulate_delta + cur_delta, cur->childs[1], &cur->childs[1]);
    }
}

AABBs *aabbtree_insert_type(AABBTree *tree, AABB aabb, AABBTreeNodeType type) {
    // create new aabb saved to tree->data
    auto *data = LIST_INSERT(&tree->data, glms_mat2x3_make(aabb[0]));

    // if tree have nothing
    if (tree->root == nullptr) {
        auto *node = (_AABBTree_Node *)malloc(sizeof(_AABBTree_Node));
        node->parent = node->childs[0] = node->childs[1] = nullptr;
        node->is_self_check = false;
        vec3 margin_vec;
        glm_vec3_broadcast(tree->margin, margin_vec);
        glm_vec3_sub(aabb[0], margin_vec, node->bounds[0]);
        glm_vec3_add(aabb[1], margin_vec, node->bounds[1]);
        node->data = data;
        node->type = type;

        tree->root = node;

        return data;
    }

    // try to find best node to insert into
    AABB fat_bounds;
    vec3 margin_vec;
    glm_vec3_broadcast(tree->margin, margin_vec);
    glm_vec3_sub(aabb[0], margin_vec, fat_bounds[0]);
    glm_vec3_add(aabb[1], margin_vec, fat_bounds[1]);

    _AABBTree_FitNodeValue best;
    best.node = tree->root;
    best.link = &tree->root;
    AABB mergeBounds;
    glm_aabb_merge(fat_bounds, tree->root->bounds, mergeBounds);
    best.value = _aabb_volume(mergeBounds);

    _find_best_fit_node_helper(&best, fat_bounds, 0.0, tree->root, &tree->root);

    // construct main node hold new data
    auto *new_node = (_AABBTree_Node *)malloc(sizeof(_AABBTree_Node));
    new_node->childs[0] = new_node->childs[1] = nullptr;
    new_node->is_self_check = false;
    glm_mat2x3_copy(fat_bounds, new_node->bounds);
    new_node->data = data;
    new_node->type = type;

    // construct common parent node of best node and new node
    auto *new_parent = (_AABBTree_Node *)malloc(sizeof(_AABBTree_Node));
    new_parent->is_self_check = false;
    new_parent->data = nullptr;
    glm_aabb_merge(best.node->bounds, new_node->bounds, new_parent->bounds);
    new_parent->parent = best.node->parent;
    new_parent->childs[0] = best.node;
    new_parent->childs[1] = new_node;
    new_parent->type = new_parent->childs[0]->type & new_parent->childs[1]->type;

    // linking to new parent
    best.node->parent = new_parent;
    new_node->parent = new_parent;
    *best.link = new_parent;

    // update upper nodes bounds
    auto *ucur_node = new_parent->parent;
    while (ucur_node != nullptr) {
        ucur_node->type = ucur_node->childs[0]->type & ucur_node->childs[1]->type;
        glm_aabb_merge(ucur_node->childs[0]->bounds, ucur_node->childs[1]->bounds, ucur_node->bounds);
        ucur_node = ucur_node->parent;
    }

    return data;
}
AABBs *aabbtree_insert(AABBTree *tree, AABB aabb) {
    return aabbtree_insert_type(tree, aabb, NORMAL);
}
AABBs *aabbtree_insert_inert(AABBTree *tree, AABB aabb) {
    return aabbtree_insert_type(tree, aabb, INERT);
}

bool _handle_remove_helper(AABBs *data, _AABBTree_Node *cur, _AABBTree_Node **cur_link) {
    bool res = false;
    if (!_is_node_leaf(cur->childs[0])) {
        if (glm_aabb_contains(cur->childs[0]->bounds, data->raw)) {
            res |= _handle_remove_helper(data, cur->childs[0], &cur->childs[0]);
        }
    } else {
        if (cur->childs[0]->data == data) {
            *cur_link = cur->childs[1];
            cur->childs[1]->parent = cur->parent;
            free(cur->childs[0]);
            free(cur);
            return true;
        }
    }

    if (!_is_node_leaf(cur->childs[1])) {
        if (glm_aabb_contains(cur->childs[1]->bounds, data->raw)) {
            res |= _handle_remove_helper(data, cur->childs[1], &cur->childs[1]);
        }
    } else {
        if (cur->childs[1]->data == data) {
            *cur_link = cur->childs[0];
            cur->childs[0]->parent = cur->parent;
            free(cur->childs[1]);
            free(cur);
            return true;
        }
    }

    if (res) {
        glm_aabb_merge(cur->childs[0]->bounds, cur->childs[1]->bounds, cur->bounds);
        cur->type = cur->childs[0]->type & cur->childs[1]->type;
    }

    return res;
}

bool aabbtree_remove(AABBTree *tree, AABBs *data) {
    if (tree->root == nullptr) return false;
    if (_is_node_leaf(tree->root)) {
        if (data == tree->root->data) {
            tree->root = nullptr;
            free(tree->root);
            return true;
        }
        return false;
    }
    return _handle_remove_helper(data, tree->root, &tree->root);
}

void _get_invalid_nodes_helper(ArrList_uintptr_t *invalid_list, _AABBTree_Node *cur) {
    if (_is_node_leaf(cur)) {
        if (!glm_aabb_contains(cur->bounds, cur->data->raw)) {
            ARRLIST_APPEND(invalid_list, (uintptr_t)cur);
        }
    } else {
        _get_invalid_nodes_helper(invalid_list, cur->childs[0]);
        _get_invalid_nodes_helper(invalid_list, cur->childs[1]);
    }
}

void _handle_reinsert_node(AABBTree *tree, _AABBTree_Node *node) {
    // pretty similar code to aabb_insert but will not realloc the node itself
    auto aabb = node->data->raw;
    if (tree->root == nullptr) {
        vec3 margin_vec;
        glm_vec3_broadcast(tree->margin, margin_vec);
        glm_vec3_sub(aabb[0], margin_vec, node->bounds[0]);
        glm_vec3_add(aabb[1], margin_vec, node->bounds[1]);
        // no need to reassign node->data

        node->parent = nullptr;

        tree->root = node;

        return;
    }

    AABB fat_bounds;
    vec3 margin_vec;
    glm_vec3_broadcast(tree->margin, margin_vec);
    glm_vec3_sub(aabb[0], margin_vec, fat_bounds[0]);
    glm_vec3_add(aabb[1], margin_vec, fat_bounds[1]);

    _AABBTree_FitNodeValue best;
    best.node = tree->root;
    best.link = &tree->root;
    AABB mergeBounds;
    glm_aabb_merge(fat_bounds, tree->root->bounds, mergeBounds);
    best.value = _aabb_volume(mergeBounds);

    _find_best_fit_node_helper(&best, fat_bounds, 0.0, tree->root, &tree->root);

    // just need to recalculate node bounds
    glm_mat2x3_copy(fat_bounds, node->bounds);

    // construct common parent node
    auto *new_parent = (_AABBTree_Node *)malloc(sizeof(_AABBTree_Node));
    new_parent->is_self_check = false;
    new_parent->data = nullptr;
    glm_aabb_merge(best.node->bounds, node->bounds, new_parent->bounds);
    new_parent->parent = best.node->parent;
    new_parent->childs[0] = best.node;
    new_parent->childs[1] = node;
    new_parent->type = new_parent->childs[0]->type & new_parent->childs[1]->type;

    best.node->parent = new_parent;
    node->parent = new_parent;
    *best.link = new_parent;

    // update upper nodes bounds
    auto *ucur_node = new_parent->parent;
    while (ucur_node != nullptr) {
        ucur_node->type = ucur_node->childs[0]->type & ucur_node->childs[1]->type;
        glm_aabb_merge(ucur_node->childs[0]->bounds, ucur_node->childs[1]->bounds, ucur_node->bounds);
        ucur_node = ucur_node->parent;
    }
}

void aabbtree_update(AABBTree *tree) {
    if (tree->root == nullptr) return;

    // one node tree?
    if (_is_node_leaf(tree->root)) {
        if (!glm_aabb_contains(tree->root->bounds, tree->root->data->raw)) {
            vec3 margin_vec;
            glm_vec3_broadcast(tree->margin, margin_vec);
            glm_vec3_sub(tree->root->data->raw[0], margin_vec, tree->root->bounds[0]);
            glm_vec3_sub(tree->root->data->raw[1], margin_vec, tree->root->bounds[1]);
        }
        return;
    }

    // invalid nodes are node data bounds is not contained in their fat bounds
    ArrList_uintptr_t invalid_list = {};
    _get_invalid_nodes_helper(&invalid_list, tree->root);

    for (int i = 0; i < invalid_list.count; i++) {
        auto *node = (_AABBTree_Node *)invalid_list.buffer[i];
        auto *parent = node->parent;
        auto *sibling = node == parent->childs[0] ? parent->childs[1] : parent->childs[0];
        auto **parent_link = parent->parent
                                 ? (parent == parent->parent->childs[0] ? &parent->parent->childs[0] : &parent->parent->childs[1])
                                 : &tree->root;

        sibling->parent = parent->parent ? parent->parent : nullptr;
        *parent_link = sibling;

        // just free the old common parent, the invalid node will be reinsert
        free(parent);

        _handle_reinsert_node(tree, node);
    }

    ARRLIST_FREE(&invalid_list);
}

void _uncheck_selfcheck_flag_helper(_AABBTree_Node *cur) {
    cur->is_self_check = false;

    if (_is_node_leaf(cur)) return;
    _uncheck_selfcheck_flag_helper(cur->childs[0]);
    _uncheck_selfcheck_flag_helper(cur->childs[1]);
}

void _get_collided_pairs_helper(AABBTreeDataPairList *list, _AABBTree_Node *node0, _AABBTree_Node *node1);
void _handle_self_collide_pair(AABBTreeDataPairList *list, _AABBTree_Node *node) {
    if (!node->is_self_check) {
        _get_collided_pairs_helper(list, node->childs[0], node->childs[1]);
        node->is_self_check = true;
    }
}
void _get_collided_pairs_helper(AABBTreeDataPairList *list, _AABBTree_Node *node0, _AABBTree_Node *node1) {
    if (node0->type == INERT && node1->type == INERT) return;

    if (_is_node_leaf(node0) && _is_node_leaf(node1)) {
        if (glm_aabb_aabb(node0->data->raw, node1->data->raw)) {
            AABBTreeDataPair pair = {node0->data, node1->data};
            ARRLIST_APPEND(list, pair);
        }
        return;
    }

    if (!glm_aabb_aabb(node0->bounds, node1->bounds)) {
        if (!_is_node_leaf(node0) && node0->type != INERT) _handle_self_collide_pair(list, node0);
        if (!_is_node_leaf(node1) && node1->type != INERT) _handle_self_collide_pair(list, node1);

        return;
    }

    if (_is_node_leaf(node0)) {
        if (node1->type != INERT) _handle_self_collide_pair(list, node1);

        _get_collided_pairs_helper(list, node0, node1->childs[0]);
        _get_collided_pairs_helper(list, node0, node1->childs[1]);

        return;
    }
    if (_is_node_leaf(node1)) {
        if (node0->type != INERT) _handle_self_collide_pair(list, node0);

        _get_collided_pairs_helper(list, node0->childs[0], node1);
        _get_collided_pairs_helper(list, node0->childs[1], node1);

        return;
    }

    if (node0->type != INERT) _handle_self_collide_pair(list, node0);
    if (node1->type != INERT) _handle_self_collide_pair(list, node1);

    _get_collided_pairs_helper(list, node0->childs[0], node1->childs[0]);
    _get_collided_pairs_helper(list, node0->childs[0], node1->childs[1]);
    _get_collided_pairs_helper(list, node0->childs[1], node1->childs[0]);
    _get_collided_pairs_helper(list, node0->childs[1], node1->childs[1]);
}

AABBTreeDataPairList aabbtree_get_collided_pairs(AABBTree *tree) {
    AABBTreeDataPairList res = {};
    if (tree->root == nullptr) return res;
    if (_is_node_leaf(tree->root)) return res;

    _uncheck_selfcheck_flag_helper(tree->root);
    _get_collided_pairs_helper(&res, tree->root->childs[0], tree->root->childs[1]);

    return res;
}

void _free_tree_helper(_AABBTree_Node *cur) {
    if (cur == nullptr) return;

    if (cur->childs[0] != nullptr) _free_tree_helper(cur->childs[0]);
    if (cur->childs[1] != nullptr) _free_tree_helper(cur->childs[1]);

    free(cur);
}

void aabbtree_free(AABBTree *tree) {
    _free_tree_helper(tree->root);
    LIST_FREE(&tree->data);
}
