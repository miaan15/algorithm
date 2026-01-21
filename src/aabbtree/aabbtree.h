#ifndef AABBTREE_H
#define AABBTREE_H

#include <cglm/struct.h>
#include <cglm/types.h>

#include "arrlist.h"
#include "list.h"

typedef mat2x3 AABB;
typedef mat2x3s AABBs;

DEFINE_LIST(AABBs)

typedef enum { NORMAL = 0, INERT = 1 } AABBTreeNodeType;

typedef struct _AABBTree_Node _AABBTree_Node;
struct _AABBTree_Node {
    _AABBTree_Node *parent;
    _AABBTree_Node *childs[2];

    AABB bounds;
    AABBs *data;

    uint8_t type : 1;
    uint8_t is_self_check : 1;
    uint8_t __padding : 6;
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

typedef union {
    AABBs *raw[2];
    struct {
        AABBs *first;
        AABBs *second;
    };
} AABBTreeDataPair;

DEFINE_ARRLIST(AABBTreeDataPair)
typedef ArrList_AABBTreeDataPair AABBTreeDataPairList;

AABBs *aabbtree_insert_type(AABBTree *tree, AABB aabb, AABBTreeNodeType type);
void aabbtree_update(AABBTree *tree);
AABBTreeDataPairList aabbtree_get_collided_pairs(AABBTree *tree);

AABBs *aabbtree_insert(AABBTree *tree, AABB aabb);
AABBs *aabbtree_insert_inert(AABBTree *tree, AABB aabb);

#endif
