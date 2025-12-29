#include "../define.h"

#define DEFINE_VECTOR2(T, Suffix)                                                                  \
    typedef struct {                                                                               \
        T x, y;                                                                                    \
    } Vector2##Suffix;                                                                             \
                                                                                                   \
    Vector2##Suffix _vector2_add_##Suffix(Vector2##Suffix a, Vector2##Suffix b) {                  \
        return (Vector2##Suffix){a.x + b.x, a.y + b.y};                                            \
    }

DEFINE_VECTOR2(F32, F)
DEFINE_VECTOR2(I32, I)
DEFINE_VECTOR2(U32, U)
