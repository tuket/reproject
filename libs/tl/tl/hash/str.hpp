#pragma once

#include "hash.hpp"
#include <tl/str.hpp>

namespace tl
{

template <>
struct hash<CStr>
{
    size_t operator()(CStr str)const noexcept {
        return hashBytes(str.c_str(), str.size());
    }
};

template <>
struct hash<Str>
{
    size_t operator()(const Str& str)const noexcept {
        return hashBytes(str.c_str(), str.size());
    }
};

template <>
struct hash<const char*>
{
    size_t operator()(const char* str)const noexcept {
        return hashBytes(str, strlen(str));
    }
};

}
