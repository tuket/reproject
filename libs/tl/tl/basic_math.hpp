#pragma once

#include "int_types.hpp"

namespace tl
{

// computes the next power of two, unless it's already a power of two
template <typename T>
static constexpr T nextPowerOf2(T x) noexcept
{
    x--;
    x |= x >> T(1);
    x |= x >> T(2);
    x |= x >> T(4);
    if constexpr(sizeof(T) >= 2)
        x |= x >> T(8);
    if constexpr(sizeof(T) >= 4)
        x |= x >> T(16);
    if constexpr(sizeof(T) >= 8)
        x |= x >> T(32);
    if constexpr(sizeof(T) >= 16)
        x |= x >> T(64);
    x++;
    return x;
}

template <typename T>
static constexpr bool isPowerOf2(T x)
{
    if(x == T(0))
        return false;
    return ((x - T(1)) & x) == T(0);
}

template <typename T>
static constexpr T pow2(T a) { return a * a; }

template <typename T>
static i8 sign(T x)
{
    return x < 0 ? -1 :
           x > 0 ? +1 : 0;
}

template <typename T>
static i8 relSign(T reference, T x) {
    return
        x < reference ? -1 :
        x > reference ? +1 : 0;
}

}
