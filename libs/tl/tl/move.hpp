#pragma once

#include "type_traits/remove_reference.hpp"
//#include "type_traits/is_reference.hpp"

namespace tl
{

template<typename T>
constexpr typename remove_reference<T>::type&&
move(T&& t) noexcept
{ return (typename remove_reference<T>::type&&)(t); }

template<typename T>
constexpr T&&
forward(typename remove_reference<T>::type& t) noexcept
{ return static_cast<T&&>(t); }

template<typename T>
constexpr T&&
forward(typename remove_reference<T>::type&& t) noexcept
{
    //static_assert(!is_lvalue_reference<T>::value, "template argument substituting T is an lvalue reference type");
    return static_cast<T&&>(t);
}

}
