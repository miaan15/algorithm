#include "../define.h"

#include <stdlib.h>

typedef struct {
    void *buffer;
    USIZE cur_addr;
    USIZE capacity;
} Arena;

void arena_new(Arena *arena, USIZE buffer_size) {
    arena->capacity = buffer_size;
    arena->buffer = malloc(arena->capacity);
    arena->cur_addr = 0;
}

void *arena_alloc(Arena *arena, USIZE size, USIZE align) {
    if (arena->cur_addr >= arena->capacity) {
        return nullptr;
    }

    if (align == 0 || (align & (align - 1)) != 0) {
        return nullptr;
    }

    UPTR addr = (UPTR)arena->buffer + (UPTR)arena->cur_addr;
    UPTR padding = (~addr + 1) & ((UPTR)align - 1);
    UPTR aligned_addr = addr + padding;
    if (aligned_addr + (UPTR)size > (UPTR)arena->buffer + (UPTR)arena->capacity) {
        return nullptr;
    }
    arena->cur_addr += size + (USIZE)padding;
    return (void *)aligned_addr;
}

void arena_free(Arena *arena) {
    free(arena->buffer);
    arena->cur_addr = 0;
    arena->capacity = 0;
}

#ifndef _MAIN
#include <stdio.h>

int main() {
    Arena a;

    arena_new(&a, 1001);

    char *c = (char *)arena_alloc(&a, 1, 1);
    *c = 0xAB;
    int *z = (int *)arena_alloc(&a, sizeof(int), 4);
    *z = 0x12345678;

    printf("%d", *z);

    arena_free(&a);
}
#endif
