#pragma once

#include "move.hpp"

namespace tl
{

template <typename T1, typename T2>
struct Pair
{
    typedef T1 first_type;
    typedef T2 second_type;

    Pair()noexcept {}

    Pair(const Pair& o) noexcept
        : first(o.first),
        second(o.second) {}

    Pair(Pair&& o) noexcept
        : first(tl::move(o.first))
        , second(tl::move(o.second)) {}

    Pair(const T1& first) : first(first), second() {}

    Pair(T1&& first) : first(tl::move(first)), second() {}

    Pair& operator=(const Pair& o) noexcept
    {
        first = o.first;
        second = o.second;
    }

    Pair& operator=(Pair&& o) noexcept
    {
        first = tl::move(o.first);
        second = tl::move(o.second);
    }

    template <typename TT1, typename TT2>
    Pair(TT1&& a, TT2&& b) noexcept
        : first(tl::forward<TT1>(a))
        , second(tl::forward<TT2>(b))
    {}

    T1 first;
    T2 second;
};

}
