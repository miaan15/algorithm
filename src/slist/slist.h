#ifndef SLIST_H
#define SLIST_H

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#define DEFINE_SLIST(T)                                                                                                                \
    typedef struct _SListNode_##T _SListNode_##T;                                                                                       \
    struct _SListNode_##T {                                                                                                             \
        _SListNode_##T *next;                                                                                                           \
        T data;                                                                                                                         \
    };                                                                                                                                  \
                                                                                                                                         \
    typedef struct {                                                                                                                    \
        size_t count;                                                                                                                   \
        _SListNode_##T *head;                                                                                                           \
    } SList_##T;

#define SLIST_INSERT(list, value)                                                                                                       \
    ({                                                                                                                                  \
        __typeof__((list)->head) __node = (__typeof__((list)->head))malloc(sizeof(*(list)->head));                                      \
        __typeof__(&(list)->head->data) __result = NULL;                                                                                \
                                                                                                                                         \
        if (__node != NULL) {                                                                                                           \
            __node->data = (value);                                                                                                     \
            __node->next = (list)->head;                                                                                                \
            (list)->head = __node;                                                                                                      \
            (list)->count++;                                                                                                            \
            __result = &__node->data;                                                                                                   \
        }                                                                                                                               \
                                                                                                                                         \
        __result;                                                                                                                       \
    })

#define SLIST_POP(list)                                                                                                                 \
    do {                                                                                                                                \
        if ((list)->head == NULL) break;                                                                                                \
                                                                                                                                         \
        __typeof__((list)->head) __old_head = (list)->head;                                                                             \
        (list)->head = (list)->head->next;                                                                                              \
        (list)->count--;                                                                                                                \
        free(__old_head);                                                                                                               \
    } while (0)

#define SLIST_REMOVE(list, value_ptr)                                                                                                   \
    do {                                                                                                                                \
        __typeof__((list)->head) __cur_node = (list)->head;                                                                             \
        __typeof__((list)->head) __pre_node = NULL;                                                                                     \
                                                                                                                                         \
        while (__cur_node != NULL) {                                                                                                    \
            if (&__cur_node->data == (value_ptr)) {                                                                                     \
                if (__pre_node != NULL) {                                                                                               \
                    __pre_node->next = __cur_node->next;                                                                                \
                } else {                                                                                                                \
                    (list)->head = __cur_node->next;                                                                                    \
                }                                                                                                                       \
                                                                                                                                         \
                (list)->count--;                                                                                                        \
                free(__cur_node);                                                                                                       \
                break;                                                                                                                  \
            }                                                                                                                           \
                                                                                                                                         \
            __pre_node = __cur_node;                                                                                                    \
            __cur_node = __cur_node->next;                                                                                              \
        }                                                                                                                               \
    } while (0)

#define SLIST_GET(list, index)                                                                                                          \
    ({                                                                                                                                  \
        __typeof__(&(list)->head->data) __result = NULL;                                                                                \
                                                                                                                                         \
        if ((index) < (list)->count) {                                                                                                  \
            __typeof__((list)->head) __cur_node = (list)->head;                                                                         \
            for (size_t __i = 0; __i < (index); __i++) {                                                                                \
                __cur_node = __cur_node->next;                                                                                          \
            }                                                                                                                           \
            __result = &__cur_node->data;                                                                                               \
        }                                                                                                                               \
                                                                                                                                         \
        __result;                                                                                                                       \
    })

#define SLIST_FREE(list)                                                                                                                \
    do {                                                                                                                                \
        __typeof__((list)->head) __cur_node = (list)->head;                                                                             \
        while (__cur_node != NULL) {                                                                                                    \
            __typeof__((list)->head) __next = __cur_node->next;                                                                         \
            free(__cur_node);                                                                                                           \
            __cur_node = __next;                                                                                                        \
        }                                                                                                                               \
                                                                                                                                         \
        (list)->head = NULL;                                                                                                            \
        (list)->count = 0;                                                                                                              \
    } while (0)

#endif
