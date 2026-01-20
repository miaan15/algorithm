#ifndef POOL_H
#define POOL_H

#include <stdlib.h>
#include <string.h>

#define DEFINE_POOL(T)                                                                                                                 \
    typedef struct {                                                                                                                   \
        union {                                                                                                                        \
            size_t next;                                                                                                               \
            T data;                                                                                                                    \
        };                                                                                                                             \
        bool valid;                                                                                                                    \
    } _PoolNode_##T;                                                                                                                  \
                                                                                                                                       \
    typedef struct {                                                                                                                   \
        size_t capacity;                                                                                                               \
        size_t count;                                                                                                                  \
        size_t max_count;                                                                                                              \
        size_t head;                                                                                                                   \
        _PoolNode_##T *buffer;                                                                                                        \
    } Pool_##T;

#define _POOL_HANDLE_CAPACITY_EXTEND(pool, size_increase)                                                                              \
    do {                                                                                                                               \
        if ((pool)->max_count + (size_increase) > (pool)->capacity) {                                                                  \
            size_t new_capacity = (pool)->capacity < 3 ? 3 : (pool)->capacity + ((pool)->capacity >> 1);                               \
                                                                                                                                       \
            if (new_capacity < (pool)->max_count + (size_increase)) {                                                                  \
                new_capacity = (pool)->max_count + (size_increase);                                                                    \
                new_capacity += (new_capacity >> 1);                                                                                   \
            }                                                                                                                          \
                                                                                                                                       \
            POOL_RESERVE((pool), new_capacity);                                                                                        \
        }                                                                                                                              \
    } while (0)

#define POOL_IS_VALID(pool, index)                                                                                                     \
    ({                                                                                                                                 \
        bool __result = false;                                                                                                         \
        if ((index) < (pool)->max_count) {                                                                                             \
            __result = (pool)->buffer[index].valid;                                                                                    \
        }                                                                                                                              \
        __result;                                                                                                                      \
    })

#define POOL_RESERVE(pool, new_capacity)                                                                                               \
    do {                                                                                                                               \
        size_t __old_capacity = (pool)->capacity;                                                                                      \
        typeof((pool)->buffer) __old_buffer = (pool)->buffer;                                                                          \
                                                                                                                                       \
        (pool)->capacity = (new_capacity);                                                                                             \
        (pool)->buffer = (typeof((pool)->buffer))malloc((pool)->capacity * sizeof(*(pool)->buffer));                                   \
                                                                                                                                       \
        if (__old_buffer != nullptr) {                                                                                                    \
            memcpy((pool)->buffer, __old_buffer, __old_capacity * sizeof(*(pool)->buffer));                                            \
            free(__old_buffer);                                                                                                        \
        }                                                                                                                              \
    } while (0)

#define POOL_INSERT(pool, value)                                                                                                       \
    ({                                                                                                                                 \
        if ((pool)->head >= (pool)->max_count) {                                                                                       \
            _POOL_HANDLE_CAPACITY_EXTEND((pool), 1);                                                                                   \
        }                                                                                                                              \
                                                                                                                                       \
        size_t __old_head = (pool)->head;                                                                                              \
        if ((pool)->head >= (pool)->max_count) {                                                                                       \
            (pool)->head++;                                                                                                            \
            (pool)->max_count++;                                                                                                       \
        } else {                                                                                                                       \
            (pool)->head = (pool)->buffer[(pool)->head].next;                                                                          \
        }                                                                                                                              \
                                                                                                                                       \
        (pool)->buffer[__old_head].valid = true;                                                                                       \
        (pool)->buffer[__old_head].data = (value);                                                                                     \
        (pool)->count++;                                                                                                               \
                                                                                                                                       \
        __old_head;                                                                                                                    \
    })

#define POOL_REMOVE(pool, index)                                                                                                       \
    do {                                                                                                                               \
        if ((index) >= (pool)->max_count) break;                                                                                       \
                                                                                                                                       \
        (pool)->buffer[index].valid = false;                                                                                           \
        (pool)->buffer[index].next = (pool)->head;                                                                                     \
        (pool)->head = (index);                                                                                                        \
        (pool)->count--;                                                                                                               \
                                                                                                                                       \
        if ((index) == (pool)->max_count - 1) {                                                                                        \
            while ((pool)->max_count > 0 && !(pool)->buffer[(pool)->max_count - 1].valid) {                                            \
                (pool)->max_count--;                                                                                                   \
            }                                                                                                                          \
        }                                                                                                                              \
    } while (0)

#define POOL_CLEAR(pool)                                                                                                               \
    do {                                                                                                                               \
        (pool)->count = 0;                                                                                                             \
        (pool)->max_count = 0;                                                                                                         \
        (pool)->head = 0;                                                                                                              \
    } while (0)

#define POOL_TRIM(pool)                                                                                                                \
    do {                                                                                                                               \
        if ((pool)->max_count == 0) {                                                                                                  \
            free((pool)->buffer);                                                                                                      \
            (pool)->buffer = nullptr;                                                                                                     \
            (pool)->capacity = 0;                                                                                                      \
            break;                                                                                                                     \
        }                                                                                                                              \
                                                                                                                                       \
        (pool)->capacity = (pool)->max_count;                                                                                          \
        (pool)->buffer = (typeof((pool)->buffer))realloc((pool)->buffer, (pool)->capacity * sizeof(*(pool)->buffer));                  \
    } while (0)

#define POOL_FREE(pool)                                                                                                                \
    do {                                                                                                                               \
        if ((pool)->buffer == nullptr) break;                                                                                             \
                                                                                                                                       \
        free((pool)->buffer);                                                                                                          \
        (pool)->buffer = nullptr;                                                                                                         \
        (pool)->count = 0;                                                                                                             \
        (pool)->max_count = 0;                                                                                                         \
        (pool)->head = 0;                                                                                                              \
        (pool)->capacity = 0;                                                                                                          \
    } while (0)

#endif
