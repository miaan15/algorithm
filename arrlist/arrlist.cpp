#include <cstdlib>
#include <cstring>

#include "define.hpp"

namespace mia {

template <typename T>
struct ArrList {
    usize count;
    usize capacity;
    T *buffer;
};

template <typename T>
T *begin(ArrList<T> &arrlist) {
    return arrlist.buffer;
}
template <typename T>
T *end(ArrList<T> &arrlist) {
    return arrlist.buffer + arrlist.count;
}
template <typename T>
const T *begin(ArrList<T> const &arrlist) {
    return arrlist.buffer;
}
template <typename T>
const T *end(ArrList<T> const &arrlist) {
    return arrlist.buffer + arrlist.count;
}
template <typename T>
const T *cbegin(ArrList<T> const &arrlist) {
    return arrlist.buffer;
}
template <typename T>
const T *cend(ArrList<T> const &arrlist) {
    return arrlist.buffer + arrlist.count;
}

template <typename T>
void arrlist_reserve(ArrList<T> *arrlist, usize new_capacity) {
    cauto old_capacity = arrlist->capacity;
    cauto old_buffer = arrlist->buffer;

    arrlist->capacity = new_capacity;
    arrlist->buffer = (T *)malloc(arrlist->capacity * sizeof(T));
    if (old_buffer != nullptr) {
        memcpy(arrlist->buffer, old_buffer, old_capacity * sizeof(T));
        free(old_buffer);
    }
}

template <typename T>
void _arrlist_capacity_extend_handle(ArrList<T> *arrlist, usize size_increase) {
    if (arrlist->count + size_increase > arrlist->capacity) {
        usize new_capacity = arrlist->capacity < 3
                                 ? 3
                                 : arrlist->capacity + (arrlist->capacity >> 1);

        if (new_capacity < arrlist->count + size_increase) {
            new_capacity = arrlist->count + size_increase;
            new_capacity += (new_capacity >> 1);
        }

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
        arrlist->buffer[i + 1] = arrlist->buffer[i];
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
void arrlist_trim(ArrList<T> *arrlist) {
    if (arrlist->count == 0) {
        free(arrlist->buffer);
        arrlist->buffer = nullptr;
        arrlist->capacity = 0;
        return;
    }

    arrlist->capacity = arrlist->count;
    arrlist->buffer =
        (T *)realloc(arrlist->buffer, arrlist->capacity * sizeof(T));
}

template <typename T>
void arrlist_free(ArrList<T> *arrlist) {
    if (arrlist->buffer == nullptr)
        return;

    free(arrlist->buffer);
    arrlist->buffer = nullptr;
    arrlist->count = 0;
    arrlist->capacity = 0;
}

} // namespace mia
