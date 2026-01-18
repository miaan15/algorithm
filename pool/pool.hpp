#ifndef POOL_HPP
#define POOL_HPP

#include "define.hpp"

namespace mia {

namespace _pool {
template <typename T>
struct Node {
    union {
        usize next;
        T data;
    };
    bool valid;
};
} // namespace _pool

template <typename T>
struct Pool {
    usize capacity;
    usize count;
    usize max;
    usize head;
    _pool::Node<T> *buffer;

    T &operator[](usize index) {
        return this->buffer[index].data;
    }
};

template <typename T>
struct PoolIterator {
    _pool::Node<T> *buffer;
    usize index;
    usize max;

    PoolIterator(_pool::Node<T> *buf, usize idx, usize m) : buffer(buf), index(idx), max(m) {
        while (index < max && !buffer[index].valid) {
            index++;
        }
    }

    T &operator*() {
        return buffer[index].data;
    }

    T *operator->() {
        return &buffer[index].data;
    }

    PoolIterator &operator++() {
        index++;
        while (index < max && !buffer[index].valid) {
            index++;
        }
        return *this;
    }

    PoolIterator operator++(int) {
        PoolIterator tmp = *this;
        ++(*this);
        return tmp;
    }

    bool operator==(const PoolIterator &other) const {
        return index == other.index;
    }

    bool operator!=(const PoolIterator &other) const {
        return index != other.index;
    }

    usize get_index() const {
        return index;
    }
};

template <typename T>
PoolIterator<T> begin(Pool<T> &pool);

template <typename T>
PoolIterator<T> end(Pool<T> &pool);

namespace pool {
template <typename T>
bool is_valid(Pool<T> *pool, usize index);

template <typename T>
void reserve(Pool<T> *pool, usize new_capacity);

template <typename T>
auto insert(Pool<T> *pool, const T &value) -> usize;

template <typename T>
void remove(Pool<T> *pool, usize index);

template <typename T>
void clear(Pool<T> *pool);

template <typename T>
void trim(Pool<T> *pool);

template <typename T>
void free(Pool<T> *pool);
} // namespace pool

} // namespace mia

#ifdef POOL_IMPLEMENTATION
#include "pool.cpp"
#endif

#endif // POOL_HPP
