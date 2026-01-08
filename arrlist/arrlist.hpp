#ifndef ARRLIST_HPP
#define ARRLIST_HPP

#include "../define.hpp"

namespace mia {

template <typename T>
struct ArrList;

template <typename T>
void arrlist_reserve(ArrList<T> *arrlist, usize new_capacity);

template <typename T>
void arrlist_append(ArrList<T> *arrlist, const T *value);

template <typename T>
void arrlist_insert(ArrList<T> *arrlist, usize index, const T *value);

template <typename T>
void arrlist_pop(ArrList<T> *arrlist);

template <typename T>
void arrlist_clear(ArrList<T> *arrlist);

template <typename T>
void trim(ArrList<T> *arrlist);

} // namespace mia

#ifndef ARRLIST_IMPLEMENTED
#include "arrlist.cpp"
#endif

#endif // ARRLIST_HPP
