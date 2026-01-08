#ifndef ARENA_HPP
#define ARENA_HPP

#include "../define.hpp"

namespace mia {

struct ArenaRegion;

struct Arena;

void *arena_alloc(Arena *arena, usize size_bytes);
void *arena_realloc(Arena *arena, const void *old_ptr, usize old_size_bytes,
                    usize new_size_bytes);
void arena_free(Arena *arena);
void arena_reset(Arena *arena);
void arena_trim_excessed(Arena *arena);

} // namespace mia
  
#ifndef ARENA_IMPLEMENTED
#include "arena.cpp"
#endif

#endif // ARENA_HPP
