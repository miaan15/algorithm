#ifndef DEFINE_HPP
#define DEFINE_HPP

#include <cstddef>
#include <cstdint>

#define cauto const auto

using i8 = int8_t;
using u8 = uint8_t;
using i16 = int16_t;
using u16 = uint16_t;
using i32 = int32_t;
using u32 = uint32_t;
using i64 = int64_t;
using u64 = uint64_t;

using f32 = float;
using f64 = double;

using usize = size_t;
using iptr = intptr_t;
using uptr = uintptr_t;

#ifdef _SOURCE_FILE
using namespace std;
#endif

#endif // DEFINE_HPP
