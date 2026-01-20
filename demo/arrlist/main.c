#include "arrlist.h"
#include "pool.h"

typedef struct {
    int a, b;
    char c;
    bool x;
} Foo;

DEFINE_ARRLIST(double)
DEFINE_POOL(double)

int main() {
    ArrList_double arr;
    ARRLIST_APPEND(&arr, 10.0)
    ARRLIST_RESERVE(&arr, 10)
    ARRLIST_INSERT(&arr, 10, 10.0)
    ARRLIST_POP(&arr)
    ARRLIST_CLEAR(&arr)
    ARRLIST_TRIM(&arr)
    ARRLIST_FREE(&arr)

    Pool_double pool;
    POOL_IS_VALID(&pool, 2);
    POOL_RESERVE(&pool, 10);
    POOL_INSERT(&pool, 10.0);
    POOL_REMOVE(&pool, 4);
    POOL_CLEAR(&pool);
    POOL_TRIM(&pool);
    POOL_FREE(&pool);
}
