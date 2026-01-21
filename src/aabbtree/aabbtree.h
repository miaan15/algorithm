#ifndef AABBTREE_H
#define AABBTREE_H

#include "list.h"
#include <cglm/struct.h>
#include <cglm/types.h>

typedef mat2x3 AABB;
typedef mat2x3s AABBs;

DEFINE_LIST(AABBs)

typedef struct _AABBTree_Node _AABBTree_Node;
struct _AABBTree_Node {
    _AABBTree_Node *parent;
    _AABBTree_Node *childs[2];

    AABB bounds;
    AABBs *data;

    bool is_self_check;
};
typedef struct {
    _AABBTree_Node *node;
    _AABBTree_Node **link;
    float value;
} _AABBTree_FitNodeValue;

typedef struct {
    List_AABBs data;
    _AABBTree_Node *root;
    float margin;
} AABBTree;

AABBs *aabbtree_insert(AABBTree *tree, AABB aabb);
void aabbtree_update(AABBTree *tree);

#endif
