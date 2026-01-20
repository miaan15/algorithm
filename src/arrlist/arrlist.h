#ifndef ARRLIST_H
#define ARRLIST_H

#include <stdlib.h>
#include <string.h>

#include "../def.h"

#define DEFINE_ARRLIST(T)                                                                                                              \
    typedef struct {                                                                                                                   \
        size_t count;                                                                                                                  \
        size_t capacity;                                                                                                               \
        T *buffer;                                                                                                                     \
    } ArrList_##T;

DEFINE_ARRLIST(char)

#define ARRLIST_RESERVE(arrlist, new_capacity)                                                                                         \
    do {                                                                                                                               \
        (arrlist)->capacity = new_capacity;                                                                                            \
        void *new_buffer = realloc((arrlist)->buffer, new_capacity * sizeof(*(arrlist)->buffer));                                      \
        if (new_buffer == NULL) {                                                                                                      \
            break;                                                                                                                     \
        }                                                                                                                              \
        (arrlist)->buffer = new_buffer;                                                                                                \
    } while (0);

#define _ARRLIST_HANDLE_CAPACITY_EXTEND(arrlist, size_increase)                                                                        \
    do {                                                                                                                               \
        if ((arrlist)->count + size_increase > (arrlist)->capacity) {                                                                  \
            size_t new_capacity = (arrlist)->capacity < 3 ? 3 : (arrlist)->capacity + ((arrlist)->capacity >> 1);                      \
                                                                                                                                       \
            if (new_capacity < (arrlist)->count + size_increase) {                                                                     \
                new_capacity = (arrlist)->count + size_increase;                                                                       \
                new_capacity += (new_capacity >> 1);                                                                                   \
            }                                                                                                                          \
                                                                                                                                       \
            ARRLIST_RESERVE((arrlist), new_capacity);                                                                                  \
        }                                                                                                                              \
    } while (0);

#define ARRLIST_APPEND(arrlist, value)                                                                                                 \
    do {                                                                                                                               \
        _ARRLIST_HANDLE_CAPACITY_EXTEND((arrlist), 1);                                                                                 \
        (arrlist)->buffer[(arrlist)->count++] = value;                                                                                \
    } while (0);

#define ARRLIST_INSERT(arrlist, index, value)                                                                                          \
    do {                                                                                                                               \
        if (index > (arrlist)->count) break;                                                                                           \
        if ((arrlist)->count == 0) {                                                                                                   \
            ARRLIST_APPEND((arrlist), value)                                                                                           \
            break;                                                                                                                     \
        }                                                                                                                              \
                                                                                                                                       \
        _ARRLIST_HANDLE_CAPACITY_EXTEND((arrlist), 1);                                                                                 \
                                                                                                                                       \
        for (size_t i = (arrlist)->count - 1; i >= index; i--) {                                                                       \
            (arrlist)->buffer[i + 1] = (arrlist)->buffer[i];                                                                           \
        }                                                                                                                              \
        (arrlist)->buffer[index] = value;                                                                                             \
        (arrlist)->count++;                                                                                                            \
    } while (0);

#define ARRLIST_APPEND_STR(arrlist, string)                                                                                            \
    do {                                                                                                                               \
        /* TODO */                                                                                                                     \
    } while (0);

#define ARRLIST_POP(arrlist)                                                                                                           \
    do {                                                                                                                               \
        if ((arrlist)->count < 1) break;                                                                                               \
        (arrlist)->count--;                                                                                                            \
    } while (0);

#define ARRLIST_CLEAR(arrlist)                                                                                                         \
    do {                                                                                                                               \
        (arrlist)->count = 0;                                                                                                          \
    } while (0);

#define ARRLIST_TRIM(arrlist)                                                                                                          \
    do {                                                                                                                               \
        if ((arrlist)->count == 0) {                                                                                                   \
            free((arrlist)->buffer);                                                                                                   \
            (arrlist)->buffer = nullptr;                                                                                               \
            (arrlist)->capacity = 0;                                                                                                   \
            break;                                                                                                                     \
        }                                                                                                                              \
                                                                                                                                       \
        (arrlist)->capacity = (arrlist)->count;                                                                                        \
        (arrlist)->buffer = realloc((arrlist)->buffer, (arrlist)->capacity * sizeof(*(arrlist)->buffer));                      \
    } while (0);

#define ARRLIST_FREE(arrlist)                                                                                                          \
    do {                                                                                                                               \
        if ((arrlist)->buffer == nullptr) break;                                                                                       \
                                                                                                                                       \
        free((arrlist)->buffer);                                                                                                       \
        (arrlist)->buffer = nullptr;                                                                                                   \
        (arrlist)->count = 0;                                                                                                          \
        (arrlist)->capacity = 0;                                                                                                       \
    } while (0);

#endif
