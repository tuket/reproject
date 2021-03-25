#pragma once

#include "add_reference.hpp"

namespace tl
{

template <typename T>
typename add_rvalue_reference<T>::type declval()noexcept;

}
