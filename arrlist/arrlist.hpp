#ifndef ARRLIST_HPP
#define ARRLIST_HPP

#include "define.hpp"

namespace mia {

template <typename T>
struct ArrList {
    usize count;
    usize capacity;
    T *buffer;
};

namespace arrlist {
template <typename T>
void reserve(ArrList<T> *arrlist, usize new_capacity);

template <typename T>
void append(ArrList<T> *arrlist, const T *value);

template <typename T>
void insert(ArrList<T> *arrlist, usize index, const T *value);

template <typename T>
void pop(ArrList<T> *arrlist);

template <typename T>
void clear(ArrList<T> *arrlist);

template <typename T>
void trim(ArrList<T> *arrlist);

template <typename T>
void free(ArrList<T> *arrlist);
} // namespace arrlist

} // namespace mia

#ifdef ARRLIST_IMPLEMENTATION
#include "arrlist.cpp"
#endif

#endif // ARRLIST_HPP
