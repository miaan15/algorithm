#include "../define.h"

#include <stdlib.h>

#define ARRAYLIST_NEW(a, s, n)                                                                     \
    do {                                                                                           \
        if (n >= 0) {                                                                              \
            a.capacity = n;                                                                        \
            a.items = malloc(a.capacity * sizeof(*a.items));                                       \
            a.size = n;                                                                            \
            for (USIZE i = 0; i < n; i++) {                                                        \
                a.items[i] = s[i];                                                                 \
            }                                                                                      \
        }                                                                                          \
    } while (0)

#define ARRAYLIST_APPEND(a, v)                                                                     \
    do {                                                                                           \
        if (a.size >= a.capacity) {                                                                \
            if (a.capacity == 0)                                                                   \
                a.capacity = 256;                                                                  \
            else                                                                                   \
                a.capacity *= 2;                                                                   \
            a.items = realloc(a.items, a.capacity * sizeof(*a.items));                             \
        }                                                                                          \
        a.items[a.size++] = v;                                                                     \
    } while (0)

#define ARRAYLIST_REMOVE_BACK(a)                                                                   \
    do {                                                                                           \
        if (a.size > 0)                                                                            \
            a.size--;                                                                              \
    } while (0)

#define ARRAYLIST_RESERVE(a, n)                                                                    \
    do {                                                                                           \
        if (n >= 0) {                                                                              \
            if (n > a.capacity) {                                                                  \
                a.items = realloc(a.items, n * sizeof(*a.items));                                  \
            }                                                                                      \
            a.capacity = n;                                                                        \
            if (a.size > a.capacity)                                                               \
                a.size = a.capacity;                                                               \
        }                                                                                          \
    } while (0)

#ifndef _MAIN
#include <stdio.h>

typedef struct {
    I32 *items;
    USIZE size;
    USIZE capacity;
} ArrayListI32;

int main() {
    ArrayListI32 a;

    int s[] = {1, 4, 6, 8};
    int n = 4;

    ARRAYLIST_NEW(a, s, n);

    for (int i = 0; i < 11; i++)
        ARRAYLIST_APPEND(a, i * 3);

    ARRAYLIST_REMOVE_BACK(a);

    for (USIZE i = 0; i < a.size; i++)
        printf("%d\n", a.items[i]);
}
#endif
