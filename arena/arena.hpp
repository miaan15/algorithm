#ifndef ARENA_HPP
#define ARENA_HPP

#include "define.hpp"

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

namespace arena {
void *alloc(Arena *arena, usize size_bytes);
void *realloc(Arena *arena, const void *old_ptr, usize old_size_bytes, usize new_size_bytes);
void free(Arena *arena);
void reset(Arena *arena);
void trim_excessed(Arena *arena);
} // namespace arena

} // namespace mia

#ifdef ARENA_IMPLEMENTATION
#include "arena.cpp"
#endif

#endif // ARENA_HPP
