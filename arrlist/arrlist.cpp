#define _SOURCE_FILE
#include "../define.hpp"

#include <cstdlib>
#include <cstring>

namespace mia {

template <typename T>
struct ArrList {
    usize count;
    usize capacity;
    T *buffer;
};

template <typename T>
void arrlist_reserve(ArrList<T> *arrlist, usize new_capacity) {
    cauto old_capacity = arrlist->capacity;
    cauto old_buffer = arrlist->buffer;

    arrlist->capacity = new_capacity;
    arrlist->buffer = (T *)malloc(arrlist->capacity);
    memcpy(arrlist->buffer, old_buffer, old_capacity);
    free(old_buffer);
}

template <typename T>
void _arrlist_capacity_extend_handle(ArrList<T> *arrlist, usize size_increase) {
    if (arrlist->count + size_increase > arrlist->capacity) {
        usize new_capacity = arrlist->capacity < 3
                                 ? 3
                                 : arrlist->capacity + (arrlist->capacity >> 1);

        arrlist_reserve(arrlist, new_capacity);
    }
}

template <typename T>
void arrlist_append(ArrList<T> *arrlist, const T value) {
    _arrlist_capacity_extend_handle(arrlist, 1);
    arrlist->buffer[arrlist->count++] = value;
}

template <typename T>
void arrlist_insert(ArrList<T> *arrlist, usize index, const T value) {
    if (index > arrlist->count)
        return;

    _arrlist_capacity_extend_handle(arrlist, 1);

    for (auto i = arrlist->count - 1; i >= index; i--) {
        *arrlist->buffer[i + 1] = *arrlist->buffer[i + 1];
    }
    arrlist->buffer[index] = value;
    arrlist->count++;
}

template <typename T>
void arrlist_pop(ArrList<T> *arrlist) {
    if (arrlist->count < 1)
        return;
    arrlist->count--;
}

template <typename T>
void arrlist_clear(ArrList<T> *arrlist) {
    arrlist->count = 0;
}

template <typename T>
void trim(ArrList<T> *arrlist) {
    arrlist->capacity = arrlist->count;
    arrlist->buffer = realloc(arrlist->buffer, arrlist->capacity);
}

} // namespace mia
