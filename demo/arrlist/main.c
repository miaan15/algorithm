#include "arrlist.h"

typedef struct {
    int a, b;
    char c;
    bool x;
} Foo;

DEFINE_ARRLIST(double)

int main() {
    ArrList_double arr;
    ARRLIST_APPEND(&arr, 10.0)
    ARRLIST_RESERVE(&arr, 10)
    ARRLIST_INSERT(&arr, 10, 10.0)
    ARRLIST_POP(&arr)
    ARRLIST_CLEAR(&arr)
    ARRLIST_TRIM(&arr)
    ARRLIST_FREE(&arr)
}
