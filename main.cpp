#include "arena/arena.hpp"
#include "arrlist/arrlist.hpp"
#include <cstdio>

using namespace mia;

int main() {
    // Arena test
    printf("=== Arena Test ===\n");
    Arena arena = {};

    i32 *num = (i32 *)arena_alloc(&arena, sizeof(i32));
    *num = 42;

    char *str = (char *)arena_alloc(&arena, 12);
    const char *src = "hello world";
    for (usize i = 0; i < 12; i++) {
        str[i] = src[i];
    }

    printf("Allocated int: %d\n", *num);
    printf("Allocated string: %s\n", str);

    arena_free(&arena);

    // ArrList test
    printf("\n=== ArrList Test ===\n");
    ArrList<i32> arrlist = {};

    arrlist_append(&arrlist, 10);
    arrlist_append(&arrlist, 20);
    arrlist_append(&arrlist, 30);
    arrlist_append(&arrlist, 40);
    arrlist_append(&arrlist, 50);

    printf("Count: %zu\n", arrlist.count);
    printf("Capacity: %zu\n", arrlist.capacity);
    printf("Elements: ");
    for (usize i = 0; i < arrlist.count; i++) {
        printf("%d ", arrlist.buffer[i]);
    }
    printf("\n");

    arrlist_pop(&arrlist);
    printf("After pop, count: %zu\n", arrlist.count);
    printf("Elements: ");
    for (usize i = 0; i < arrlist.count; i++) {
        printf("%d ", arrlist.buffer[i]);
    }
    printf("\n");

    arrlist_clear(&arrlist);
    printf("After clear, count: %zu\n", arrlist.count);
    printf("Elements: ");
    for (usize i = 0; i < arrlist.count; i++) {
        printf("%d ", arrlist.buffer[i]);
    }
    printf("\n");

    return 0;
}
