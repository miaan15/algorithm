#ifndef LIST_H
#define LIST_H

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#define DEFINE_LIST(T)                                                                                                                 \
    typedef struct _ListNode_##T _ListNode_##T;                                                                                        \
    struct _ListNode_##T {                                                                                                             \
        _ListNode_##T *next;                                                                                                           \
        T data;                                                                                                                        \
    };                                                                                                                                 \
                                                                                                                                       \
    typedef struct {                                                                                                                   \
        size_t count;                                                                                                                  \
        _ListNode_##T *head;                                                                                                           \
    } List_##T;

#define LIST_INSERT(list, value)                                                                                                       \
    ({                                                                                                                                 \
        typeof((list)->head) __node = malloc(sizeof(*(list)->head));                                                                   \
        typeof(&(list)->head->data) __result = NULL;                                                                                   \
                                                                                                                                       \
        if (__node != NULL) {                                                                                                          \
            __node->data = (value);                                                                                                    \
            __node->next = (list)->head;                                                                                               \
            (list)->head = __node;                                                                                                     \
            (list)->count++;                                                                                                           \
            __result = &__node->data;                                                                                                  \
        }                                                                                                                              \
                                                                                                                                       \
        __result;                                                                                                                      \
    })

#define LIST_POP(list)                                                                                                                 \
    do {                                                                                                                               \
        if ((list)->head == NULL) break;                                                                                               \
                                                                                                                                       \
        typeof((list)->head) __old_head = (list)->head;                                                                                \
        (list)->head = (list)->head->next;                                                                                             \
        (list)->count--;                                                                                                               \
        free(__old_head);                                                                                                              \
    } while (0)

#define LIST_REMOVE(list, value_ptr)                                                                                                   \
    do {                                                                                                                               \
        typeof((list)->head) __cur_node = (list)->head;                                                                                \
        typeof((list)->head) __pre_node = NULL;                                                                                        \
                                                                                                                                       \
        while (__cur_node != NULL) {                                                                                                   \
            if (&__cur_node->data == (value_ptr)) {                                                                                    \
                if (__pre_node != NULL) {                                                                                              \
                    __pre_node->next = __cur_node->next;                                                                               \
                } else {                                                                                                               \
                    (list)->head = __cur_node->next;                                                                                   \
                }                                                                                                                      \
                                                                                                                                       \
                (list)->count--;                                                                                                       \
                free(__cur_node);                                                                                                      \
                break;                                                                                                                 \
            }                                                                                                                          \
                                                                                                                                       \
            __pre_node = __cur_node;                                                                                                   \
            __cur_node = __cur_node->next;                                                                                             \
        }                                                                                                                              \
    } while (0)

#define LIST_GET(list, index)                                                                                                          \
    ({                                                                                                                                 \
        typeof(&(list)->head->data) __result = NULL;                                                                                   \
                                                                                                                                       \
        if ((index) < (list)->count) {                                                                                                 \
            typeof((list)->head) __cur_node = (list)->head;                                                                            \
            for (size_t __i = 0; __i < (index); __i++) {                                                                               \
                __cur_node = __cur_node->next;                                                                                         \
            }                                                                                                                          \
            __result = &__cur_node->data;                                                                                              \
        }                                                                                                                              \
                                                                                                                                       \
        __result;                                                                                                                      \
    })

#define LIST_FREE(list)                                                                                                                \
    do {                                                                                                                               \
        typeof((list)->head) __cur_node = (list)->head;                                                                                \
        while (__cur_node != NULL) {                                                                                                   \
            typeof((list)->head) __next = __cur_node->next;                                                                            \
            free(__cur_node);                                                                                                          \
            __cur_node = __next;                                                                                                       \
        }                                                                                                                              \
                                                                                                                                       \
        (list)->head = NULL;                                                                                                           \
        (list)->count = 0;                                                                                                             \
    } while (0)

#endif
