#pragma once

namespace tl
{

namespace internal
{
template <unsigned i, typename T, typename... Ts>
struct type_at {
    typedef type_at<i-1, Ts...> type;
};

template <typename T, typename... Ts>
struct type_at<0, T, Ts...> {
    typedef T type;
};
}

template <unsigned i, typename... Ts>
using type_at = typename internal::type_at<i, Ts...>::type;

}
