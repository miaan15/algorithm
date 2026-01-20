#include "arena.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct _ArenaRegion _ArenaRegion;
struct _ArenaRegion {
    _ArenaRegion *next;
    size_t count;
    size_t capacity;
    uintptr_t buffer[];
};

struct Arena {
    _ArenaRegion *begin, *end;
};

_ArenaRegion *_arena_new_region(size_t capacity) {
    auto size_bytes = sizeof(_ArenaRegion) + sizeof(uintptr_t) * capacity;

    auto *r = (_ArenaRegion *)malloc(size_bytes);
    r->next = nullptr;
    r->count = 0;
    r->capacity = capacity;

    return r;
}

void _arena_free_region(_ArenaRegion *region) {
    free(region);
}

void *arena_alloc(Arena *arena, size_t size_bytes) {
    constexpr auto align = sizeof(uintptr_t);
    const auto size = (size_bytes + align - 1) / align;

    if (arena->end == nullptr) {
        const auto r_capacity = (size_t)ARENA_REGION_DEFAULT_CAPACITY < size ? size : (size_t)ARENA_REGION_DEFAULT_CAPACITY;
        arena->end = _arena_new_region(r_capacity);
        arena->end->count += size;
        arena->begin = arena->end;

        return &arena->end->buffer[0];
    }

    while (arena->end->count + size > arena->end->capacity && arena->end->next != nullptr) {
        arena->end = arena->end->next;
    }

    if (arena->end->count + size > arena->end->capacity) {
        const auto r_capacity = (size_t)ARENA_REGION_DEFAULT_CAPACITY < size ? size : (size_t)ARENA_REGION_DEFAULT_CAPACITY;
        arena->end->next = _arena_new_region(r_capacity);
        arena->end = arena->end->next;
    }

    void *res = &arena->end->buffer[arena->end->count];
    arena->end->count += size;
    return res;
}

void *arena_realloc(Arena *arena, const void *old_ptr, size_t old_size_bytes, size_t new_size_bytes) {
    void *new_ptr = arena_alloc(arena, new_size_bytes);
    memcpy(new_ptr, old_ptr, old_size_bytes);
    return new_ptr;
}

void arena_free(Arena *arena) {
    auto cur_region = arena->begin;
    while (cur_region) {
        auto t = cur_region;
        cur_region = cur_region->next;
        _arena_free_region(t);
    }
    arena->begin = arena->end = nullptr;
}

inline void arena_reset(Arena *arena) {
    for (auto r = arena->begin; r != nullptr; r = r->next) {
        r->count = 0;
    }
    arena->end = arena->begin;
}

inline void arena_trim(Arena *arena) {
    auto cur_region = arena->begin->next;
    while (cur_region) {
        auto t = cur_region;
        cur_region = cur_region->next;
        _arena_free_region(t);
    }
    arena->begin->next = nullptr;
}
