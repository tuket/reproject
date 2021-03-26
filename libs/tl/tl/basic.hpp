#pragma once

#include "int_types.hpp"
#include "move.hpp"

namespace tl
{

typedef decltype(nullptr) nullptr_t;

template <typename T, size_t N>
constexpr size_t size(T(&)[N]) { return N; }

template<typename T>
void swap(T& a, T& b)
{
    T x = move(a);
    a = move(b);
    b = move(x);
}

template <typename T>
constexpr auto min(T a, T b) noexcept
{
    return a < b ? a : b;
}

template <typename T>
constexpr auto max(T a, T b) noexcept
{
    return a > b ? a : b;
}

template <typename T>
static constexpr void minMax(T& a, T& b) noexcept
{
    if(a > b)
        tl::swap(a, b);
}

template <typename T>
static constexpr T clamp(const T& x, const T& min, const T& max) noexcept
{
    return (x > min) ? (x < max ? x : max) : min;
}

}

template <typename Container, typename Element>
bool has(const Container& v, const Element& e) {
    for(const auto& x : v)
        if(x == e)
            return true;
    return false;
}

template <typename F>
struct _Defer {
    F f;
    _Defer(F f) : f(f) {}
    ~_Defer() { f(); }
};

template <typename F>
_Defer<F> _defer_func(F f) {
    return _Defer<F>(f);
}

#define DEFER_1(x, y) x##y
#define DEFER_2(x, y) DEFER_1(x, y)
#define DEFER_3(x)    DEFER_2(x, __COUNTER__)
#define defer(code)   auto DEFER_3(_defer_) = _defer_func([&](){code;})
