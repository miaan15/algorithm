#ifndef POOL_HPP
#define POOL_HPP

#include <cstdlib>
#include <cstring>

#include <define.hpp>

namespace mia {

namespace detail {
template <typename T>
struct PoolNode {
    union {
        size_t next;
        T data;
    };
    bool valid;
};
}; // namespace detail

template <typename T>
struct Pool {
    size_t capacity;
    size_t count;
    size_t max;
    size_t head;
    detail::PoolNode<T> *buffer;
};

namespace detail {
template <typename T>
void _handle_capacity_extend(Pool<T> *pool, size_t size_increase);
} // namespace detail

namespace pool {
template <typename T>
bool is_valid(Pool<T> *pool, size_t index) {
    if (index >= pool->max) return false;
    return pool->buffer[index].valid;
}

template <typename T>
void reserve(Pool<T> *pool, size_t new_capacity) {
    const auto old_capacity = pool->capacity;
    const auto old_buffer = pool->buffer;

    pool->capacity = new_capacity;
    pool->buffer = (detail::PoolNode<T> *)malloc(pool->capacity * sizeof(detail::PoolNode<T>));
    if (old_buffer != nullptr) {
        memcpy(pool->buffer, old_buffer, old_capacity * sizeof(detail::PoolNode<T>));
        ::free(old_buffer);
    }
}

template <typename T>
auto insert(Pool<T> *pool, const T &value) -> size_t {
    if (pool->head >= pool->max) detail::_handle_capacity_extend(pool, 1);

    const auto old_head = pool->head;
    if (pool->head >= pool->max) {
        pool->head++;
        pool->max++;
    } else {
        pool->head = pool->buffer[pool->head].next;
    }

    pool->buffer[old_head].valid = true;
    pool->buffer[old_head].data = value;

    pool->count++;

    return old_head;
}

template <typename T>
void remove(Pool<T> *pool, size_t index) {
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
    pool->buffer = (detail::PoolNode<T> *)realloc(pool->buffer, pool->capacity * sizeof(detail::PoolNode<T>));
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
struct PoolIterator {
    detail::PoolNode<T> *buffer;
    size_t index;
    size_t max;

    PoolIterator(detail::PoolNode<T> *buf, size_t idx, size_t m) : buffer(buf), index(idx), max(m) {
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

    size_t get_index() const {
        return index;
    }
};

template <typename T>
PoolIterator<T> begin(Pool<T> &pool) {
    return PoolIterator<T>(pool.buffer, 0, pool.max);
}

template <typename T>
PoolIterator<T> end(Pool<T> &pool) {
    return PoolIterator<T>(pool.buffer, pool.max, pool.max);
}

namespace detail {
template <typename T>
void _handle_capacity_extend(Pool<T> *pool, size_t size_increase) {
    if (pool->max + size_increase > pool->capacity) {
        size_t new_capacity = pool->capacity < 3 ? 3 : pool->capacity + (pool->capacity >> 1);

        if (new_capacity < pool->max + size_increase) {
            new_capacity = pool->max + size_increase;
            new_capacity += (new_capacity >> 1);
        }

        mia::pool::reserve(pool, new_capacity);
    }
}
} // namespace detail

} // namespace mia

#endif // POOL_HPP
