#pragma once

namespace tl
{

namespace internal
{
template <unsigned i, typename T, typename T2, typename... T2s>
struct type_index {
    static constexpr unsigned index = type_index<i+1, T, T2s...>::index;
};

template <unsigned i, typename T, typename... T2s>
struct type_index <i, T, T, T2s...> {
    static constexpr unsigned index = i;
};
}

template <typename T, typename... Ts>
constexpr unsigned typeIndex() {
    return internal::type_index<0, T, Ts...>::index;
}

}
