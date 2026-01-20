#ifndef ARENA_H
#define ARENA_H

#include <stddef.h>

#ifndef ARENA_REGION_DEFAULT_CAPACITY
#define ARENA_REGION_DEFAULT_CAPACITY (8 * 1024)
#endif

typedef struct Arena Arena;

void *arena_alloc(Arena *arena, size_t size_bytes);
void *arena_realloc(Arena *arena, const void *old_ptr, size_t old_size_bytes, size_t new_size_bytes);
void arena_free(Arena *arena);
void arena_reset(Arena *arena);
void arena_trim(Arena *arena);

#endif
