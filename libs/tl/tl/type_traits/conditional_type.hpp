#pragma once

namespace tl
{

template<bool b, typename T, typename F>
struct conditional_type {
    typedef T type;
};

template<typename T, typename F>
struct conditional_type<false, T, F> {
    typedef F type;
};

}
