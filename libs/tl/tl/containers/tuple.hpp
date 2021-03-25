#pragma once

#include <tl/move.hpp>

namespace tl
{

template <typename... Ts>
class Tuple {
public:
    constexpr Tuple(){}

    bool operator==(const Tuple<>&)const { return true; }
    bool operator!=(const Tuple<>&)const { return false; }
    bool operator<(const Tuple<>&)const { return false; }
};

template <typename T, typename... Ts>
class Tuple<T, Ts...>
    : Tuple<Ts...>
{
public:
    Tuple(){}
    Tuple(T&& x, Ts... xs)
        : first(tl::forward(x))
        , Tuple<Ts...>(tl::forward(xs)...)
    {}

    template <unsigned i>
    auto& get() {
        return Tuple<Ts...>::template get<i-1>();
    }
    template <>
    auto& get<0>() {
        return first;
    }

    bool operator==(const Tuple<T, Ts...>& o)const {
        return first == o. first && Tuple<Ts...>::operator==(o);
    }

    bool operator!=(const Tuple<T, Ts...>& o)const {
        return !operator==(o);
    }

    bool operator<(const Tuple<T, Ts...>& o) const {
        return
            first < o. first ? true :
            first > o.first ? false :
            Tuple<Ts...>::operator<(o);
    }

    T first;
};

template<unsigned i, typename T, typename... Ts>
auto get(Tuple<T, Ts...>& x)
{
    return x.template get<i>();
}

}
