#include <cstdlib>
#include <cstring>

#include "define.hpp"

namespace mia {

namespace _pool {};
using namespace _pool;

#ifndef POOL_IMPLEMENTATION
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
    Node<T> *buffer;

    T &operator[](usize index) {
        return this->buffer[index].data;
    }
};

template <typename T>
struct PoolIterator {
    Node<T> *buffer;
    usize index;
    usize max;

    PoolIterator(Node<T> *buf, usize idx, usize m) : buffer(buf), index(idx), max(m) {
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
#endif

namespace _pool {
template <typename T>
void handle_capacity_extend(Pool<T> *pool, usize size_increase);
} // namespace _pool

namespace pool {
template <typename T>
auto is_valid(Pool<T> *pool, usize index) {
    if (index >= pool->max) return false;
    return pool->buffer[index].valid;
}

template <typename T>
void reserve(Pool<T> *pool, usize new_capacity) {
    cauto old_capacity = pool->capacity;
    cauto old_buffer = pool->buffer;

    pool->capacity = new_capacity;
    pool->buffer = (Node<T> *)malloc(pool->capacity * sizeof(Node<T>));
    if (old_buffer != nullptr) {
        memcpy(pool->buffer, old_buffer, old_capacity * sizeof(T));
        ::free(old_buffer);
    }
}

template <typename T>
void insert(Pool<T> *pool, const T &value) {
    if (pool->head >= pool->max) handle_capacity_extend(pool, 1);

    cauto old_head = pool->head;
    if (pool->head >= pool->max) {
        pool->head++;
        pool->max++;
    } else {
        pool->head = pool->buffer[pool->head].next;
    }

    pool->buffer[old_head].valid = true;
    pool->buffer[old_head].data = value;

    pool->count++;
}

template <typename T>
void remove(Pool<T> *pool, usize index) {
    if (index >= pool->max) return;

    pool->buffer[index].valid = false;
    pool->buffer[index].next = pool->head;
    pool->head = index;
    pool->count--;

    if (index == pool->max - 1) {
        while (pool->max > 0 && !pool->buffer[pool->max - 1].valid) {
            pool->max--;
        }
    }
}

template <typename T>
void clear(Pool<T> *pool) {
    pool->count = 0;
    pool->max = 0;
    pool->head = 0;
}

template <typename T>
void trim(Pool<T> *pool) {
    if (pool->max == 0) {
        free(pool->buffer);
        pool->buffer = nullptr;
        pool->capacity = 0;
        return;
    }

    pool->capacity = pool->max;
    pool->buffer = (T *)realloc(pool->buffer, pool->capacity * sizeof(T));
}

template <typename T>
void free(Pool<T> *pool) {
    if (pool->buffer == nullptr) return;

    ::free(pool->buffer);
    pool->buffer = nullptr;
    pool->count = 0;
    pool->max = 0;
    pool->head = 0;
    pool->capacity = 0;
}
} // namespace pool

template <typename T>
void _pool::handle_capacity_extend(Pool<T> *pool, usize size_increase) {
    if (pool->max + size_increase > pool->capacity) {
        usize new_capacity = pool->capacity < 3 ? 3 : pool->capacity + (pool->capacity >> 1);

        if (new_capacity < pool->max + size_increase) {
            new_capacity = pool->max + size_increase;
            new_capacity += (new_capacity >> 1);
        }

        pool::reserve(pool, new_capacity);
    }
}

template <typename T>
PoolIterator<T> begin(Pool<T> &pool) {
    return PoolIterator<T>(pool.buffer, 0, pool.max);
}

template <typename T>
PoolIterator<T> end(Pool<T> &pool) {
    return PoolIterator<T>(pool.buffer, pool.max, pool.max);
}

} // namespace mia
