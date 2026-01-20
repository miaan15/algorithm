#include "aabbtree.h"

#include <cglm/vec2.h>

typedef struct _AABBTree_Node _AABBTree_Node;
struct _AABBTree_Node {
    _AABBTree_Node *parent;
    _AABBTree_Node *childs[2];

    vec2 bounds[2];
    vec2 (*data)[2];

    bool is_self_check;
};
struct AABBTree {
    _AABBTree_Node *root;
    float margin;
};
