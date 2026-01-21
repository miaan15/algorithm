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

bool _aabbtree_is_node_leaf(const _AABBTree_Node *const node) {
    return node->childs[0] == nullptr || node->childs[1] == nullptr;
}

void _aabbtree_find_best_fit_node_helper(_AABBTree_FitNodeValue *best, AABB bounds, float accumulate_delta, _AABBTree_Node *cur,
                                         _AABBTree_Node **cur_link) {
    AABB mergeBounds;
    glm_aabb_merge(bounds, cur->bounds, mergeBounds);
    float cur_value = _aabb_volume(mergeBounds) + accumulate_delta;
    if (cur_value < best->value) {
        best->node = cur;
        best->link = cur_link;
        best->value = cur_value;
    }

    if (_aabbtree_is_node_leaf(cur)) return;

    float cur_delta = _aabb_volume(mergeBounds) - _aabb_volume(cur->bounds);
    if (_aabb_volume(bounds) + cur_delta + accumulate_delta < best->value) {
        _aabbtree_find_best_fit_node_helper(best, bounds, accumulate_delta + cur_delta, cur->childs[0], &cur->childs[0]);
        _aabbtree_find_best_fit_node_helper(best, bounds, accumulate_delta + cur_delta, cur->childs[1], &cur->childs[1]);
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

    _aabbtree_find_best_fit_node_helper(&best, fat_bounds, 0.0, tree->root, &tree->root);

    // construct main node hold new data
    auto *new_node = (_AABBTree_Node *)malloc(sizeof(_AABBTree_Node));
    new_node->childs[0] = new_node->childs[1] = nullptr;
    new_node->is_self_check = false;
    glm_mat2x3_copy(fat_bounds, new_node->bounds);
    new_node->data = data;

    // construct common parent node of best node and new node
    auto *new_parent = (_AABBTree_Node *)malloc(sizeof(_AABBTree_Node));
    new_parent->is_self_check = false;
    new_parent->data = nullptr;
    glm_aabb_merge(best.node->bounds, new_node->bounds, new_parent->bounds);
    new_parent->parent = best.node->parent;
    new_parent->childs[0] = best.node;
    new_parent->childs[1] = new_node;

    // linking to new parent
    best.node->parent = new_parent;
    new_node->parent = new_parent;
    *best.link = new_parent;

    // update upper nodes bounds
    auto *ucur_node = new_parent->parent;
    while (ucur_node != nullptr) {
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

void _aabbtree_get_invalid_nodes_helper(ArrList_uintptr_t *invalid_list, _AABBTree_Node *cur) {
    if (_aabbtree_is_node_leaf(cur)) {
        if (!glm_aabb_contains(cur->bounds, cur->data->raw)) {
            ARRLIST_APPEND(invalid_list, (uintptr_t)cur);
        }
    } else {
        _aabbtree_get_invalid_nodes_helper(invalid_list, cur->childs[0]);
        _aabbtree_get_invalid_nodes_helper(invalid_list, cur->childs[1]);
    }
}

void _aabbtree_handle_reinsert_node(AABBTree *tree, _AABBTree_Node *node) {
    // pretty similar code to aabb_insert but will not realloc the node itself
    auto aabb = node->data->raw;
    if (tree->root == nullptr) {
        vec3 margin_vec;
        glm_vec3_broadcast(tree->margin, margin_vec);
        glm_vec3_sub(aabb[0], margin_vec, node->bounds[0]);
        glm_vec3_add(aabb[1], margin_vec, node->bounds[1]);
        // no need to reassign node->data

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

    _aabbtree_find_best_fit_node_helper(&best, fat_bounds, 0.0, tree->root, &tree->root);

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

    best.node->parent = new_parent;
    node->parent = new_parent;
    *best.link = new_parent;

    // update upper nodes bounds
    auto *ucur_node = new_parent->parent;
    while (ucur_node != nullptr) {
        glm_aabb_merge(ucur_node->childs[0]->bounds, ucur_node->childs[1]->bounds, ucur_node->bounds);
        ucur_node = ucur_node->parent;
    }
}

void aabbtree_update(AABBTree *tree) {
    if (tree->root == nullptr) return;

    // one node tree?
    if (_aabbtree_is_node_leaf(tree->root)) {
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
    _aabbtree_get_invalid_nodes_helper(&invalid_list, tree->root);

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

        _aabbtree_handle_reinsert_node(tree, node);
    }

    ARRLIST_FREE(&invalid_list);
}
