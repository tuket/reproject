#pragma once

namespace tl
{

template <typename T>
struct add_rvalue_reference {
    typedef T&& type;
};

template <typename T>
struct add_rvalue_reference<T&&> {
    typedef T&& type;
};

}
