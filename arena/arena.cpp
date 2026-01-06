#define _SOURCE_FILE
#include "../define.hpp"
#include <cstdlib>
#include <cstring>

#ifndef ARENA_REGION_DEFAULT_CAPACITY
#define ARENA_REGION_DEFAULT_CAPACITY (8 * 1024)
#endif

namespace mia {

struct ArenaRegion {
    ArenaRegion *next;
    usize count;
    usize capacity;
    uptr buffer[];
};

struct Arena {
    ArenaRegion *begin, *end;
};

ArenaRegion *new_region(usize capacity) {
    cauto size_bytes = sizeof(ArenaRegion) + sizeof(uptr) * capacity;

    auto *r = (ArenaRegion *)malloc(size_bytes);
    r->next = nullptr;
    r->count = 0;
    r->capacity = capacity;

    return r;
}
void free_region(ArenaRegion *region) {
    free(region);
}

void *arena_alloc(Arena *arena, usize size_bytes) {
    constexpr auto align = sizeof(uptr);
    cauto size = (size_bytes + align - 1) / align;

    if (arena->end == nullptr) {
        cauto r_capacity = (usize)ARENA_REGION_DEFAULT_CAPACITY < size
                               ? size
                               : (usize)ARENA_REGION_DEFAULT_CAPACITY;
        arena->end = new_region(r_capacity);
        arena->begin = arena->end;
        return &arena->end->buffer[0];
    }

    while (arena->end->count + size > arena->end->capacity &&
           arena->end->next != nullptr) {
        arena->end = arena->end->next;
    }

    if (arena->end->count + size > arena->end->capacity) {
        cauto r_capacity = (usize)ARENA_REGION_DEFAULT_CAPACITY < size
                               ? size
                               : (usize)ARENA_REGION_DEFAULT_CAPACITY;
        arena->end->next = new_region(r_capacity);
        arena->end = arena->end->next;
    }

    void *res = &arena->end->buffer[arena->end->count];
    arena->end->count += size;
    return res;
}

void *arena_realloc(Arena *arena, const void *old_ptr, usize old_size_bytes,
                    usize new_size_bytes) {
    void *new_ptr = arena_alloc(arena, new_size_bytes);
    memcpy(new_ptr, old_ptr, old_size_bytes);
    return new_ptr;
}

void arena_free(Arena *arena) {
    auto cur_region = arena->begin;
    while (cur_region) {
        auto t = cur_region;
        cur_region = cur_region->next;
        free_region(t);
    }
    arena->begin = arena->end = nullptr;
}

void arena_reset(Arena *arena) {
    for (auto r = arena->begin; r != nullptr; r = r->next) {
        r->count = 0;
    }
    arena->end = arena->begin;
}

void arena_trim_excessed(Arena *arena) {
    auto cur_region = arena->begin->next;
    while (cur_region) {
        auto t = cur_region;
        cur_region = cur_region->next;
        free_region(t);
    }
    arena->begin->next = nullptr;
}

} // namespace mia
