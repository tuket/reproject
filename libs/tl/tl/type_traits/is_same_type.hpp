#pragma once

namespace tl
{

template <typename T1, typename T2>
struct is_same_type {
    static constexpr bool value = false;
};

template <typename T1>
struct is_same_type<T1, T1> {
    static constexpr bool value = true;
};

}
