#ifndef ARENA_HPP
#define ARENA_HPP

#include <cstddef>
#include <cstdint>

namespace mia {

struct ArenaRegion {
    ArenaRegion *next;
    size_t count;
    size_t capacity;
    uintptr_t buffer[];
};

struct Arena {
    ArenaRegion *begin, *end;
};

void *arena_alloc(Arena *arena, size_t size_bytes);
void *arena_realloc(Arena *arena, const void *old_ptr, size_t old_size_bytes,
                    size_t new_size_bytes);
void arena_free(Arena *arena);
void arena_reset(Arena *arena);
void arena_trim_excessed(Arena *arena);

} // namespace mia

#endif // ARENA_HPP
