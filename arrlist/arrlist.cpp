#include <cstdlib>
#include <cstring>

#include "define.hpp"

namespace mia {

namespace _arrlist {}
using namespace _arrlist;

#ifndef ARRLIST_IMPLEMENTATION
template <typename T>
struct ArrList {
    usize count;
    usize capacity;
    T *buffer;

    T &operator[](usize index) {
        return this->buffer[index];
    }
};
#endif

template <typename T>
T *begin(ArrList<T> &arrlist) {
    return arrlist.buffer;
}
template <typename T>
T *end(ArrList<T> &arrlist) {
    return arrlist.buffer + arrlist.count;
}
template <typename T>
const T *begin(const ArrList<T> &arrlist) {
    return arrlist.buffer;
}
template <typename T>
const T *end(const ArrList<T> &arrlist) {
    return arrlist.buffer + arrlist.count;
}
template <typename T>
const T *cbegin(const ArrList<T> &arrlist) {
    return arrlist.buffer;
}
template <typename T>
const T *cend(const ArrList<T> &arrlist) {
    return arrlist.buffer + arrlist.count;
}

namespace _arrlist {
template <typename T>
void handle_capacity_extend(ArrList<T> *arrlist, usize size_increase);
} // namespace _arrlist

namespace arrlist {
template <typename T>
void reserve(ArrList<T> *arrlist, usize new_capacity) {
    cauto old_capacity = arrlist->capacity;
    cauto old_buffer = arrlist->buffer;

    arrlist->capacity = new_capacity;
    arrlist->buffer = (T *)malloc(arrlist->capacity * sizeof(T));
    if (old_buffer != nullptr) {
        memcpy(arrlist->buffer, old_buffer, old_capacity * sizeof(T));
        ::free(old_buffer);
    }
}

template <typename T>
void append(ArrList<T> *arrlist, const T &value) {
    handle_capacity_extend(arrlist, 1);
    arrlist->buffer[arrlist->count++] = value;
}

template <typename T>
void insert(ArrList<T> *arrlist, usize index, const T &value) {
    if (index > arrlist->count) return;

    handle_capacity_extend(arrlist, 1);

    for (auto i = arrlist->count - 1; i >= index; i--) {
        arrlist->buffer[i + 1] = arrlist->buffer[i];
    }
    arrlist->buffer[index] = value;
    arrlist->count++;
}

template <typename T>
void pop(ArrList<T> *arrlist) {
    if (arrlist->count < 1) return;
    arrlist->count--;
}

template <typename T>
void clear(ArrList<T> *arrlist) {
    arrlist->count = 0;
}

template <typename T>
void trim(ArrList<T> *arrlist) {
    if (arrlist->count == 0) {
        free(arrlist->buffer);
        arrlist->buffer = nullptr;
        arrlist->capacity = 0;
        return;
    }

    arrlist->capacity = arrlist->count;
    arrlist->buffer = (T *)realloc(arrlist->buffer, arrlist->capacity * sizeof(T));
}

template <typename T>
void free(ArrList<T> *arrlist) {
    if (arrlist->buffer == nullptr) return;

    ::free(arrlist->buffer);
    arrlist->buffer = nullptr;
    arrlist->count = 0;
    arrlist->capacity = 0;
}
} // namespace arrlist

template <typename T>
void _arrlist::handle_capacity_extend(ArrList<T> *arrlist, usize size_increase) {
    if (arrlist->count + size_increase > arrlist->capacity) {
        usize new_capacity = arrlist->capacity < 3 ? 3 : arrlist->capacity + (arrlist->capacity >> 1);

        if (new_capacity < arrlist->count + size_increase) {
            new_capacity = arrlist->count + size_increase;
            new_capacity += (new_capacity >> 1);
        }

        arrlist::reserve(arrlist, new_capacity);
    }
}

} // namespace mia
