#pragma once

#include <tl/int_types.hpp>

namespace tl
{

size_t hashBytes(const void* s, size_t size) noexcept;

size_t hashInt(uint64_t obj) noexcept;

template <typename T>
struct hash{};

template <class T>
struct hash<T*> {
    size_t operator()(T* ptr) const noexcept {
        return hashInt(reinterpret_cast<size_t>(ptr));
    }
};

#define ROBIN_HOOD_HASH_INT(T)                           \
    template <>                                          \
    struct hash<T> {                                     \
        size_t operator()(T obj) const noexcept {        \
            return hashInt(static_cast<uint64_t>(obj)); \
        }                                                \
    }

#if defined(__GNUC__) && !defined(__clang__)
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wuseless-cast"
#endif
// see https://en.cppreference.com/w/cpp/utility/hash
ROBIN_HOOD_HASH_INT(bool);
ROBIN_HOOD_HASH_INT(char);
ROBIN_HOOD_HASH_INT(signed char);
ROBIN_HOOD_HASH_INT(unsigned char);
ROBIN_HOOD_HASH_INT(char16_t);
ROBIN_HOOD_HASH_INT(char32_t);
ROBIN_HOOD_HASH_INT(wchar_t);
ROBIN_HOOD_HASH_INT(short);
ROBIN_HOOD_HASH_INT(unsigned short);
ROBIN_HOOD_HASH_INT(int);
ROBIN_HOOD_HASH_INT(unsigned int);
ROBIN_HOOD_HASH_INT(long);
ROBIN_HOOD_HASH_INT(long long);
ROBIN_HOOD_HASH_INT(unsigned long);
ROBIN_HOOD_HASH_INT(unsigned long long);
#if defined(__GNUC__) && !defined(__clang__)
#    pragma GCC diagnostic pop
#endif

}
