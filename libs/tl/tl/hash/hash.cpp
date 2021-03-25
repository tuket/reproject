#include "hash.hpp"

namespace tl
{

#ifndef __has_cpp_attribute // For backwards compatibility
#    define __has_cpp_attribute(x) 0
#endif
#if __has_cpp_attribute(clang::fallthrough)
#    define SWITCH_FALLTHROUGH [[clang::fallthrough]]
#elif __has_cpp_attribute(gnu::fallthrough)
#    define SWITCH_FALLTHROUGH [[gnu::fallthrough]]
#else
#    define SWITCH_FALLTHROUGH
#endif

// umul
#if defined(__SIZEOF_INT128__)
#    define HAS_UMUL128 1
#    if defined(__GNUC__) || defined(__clang__)
#        pragma GCC diagnostic push
#        pragma GCC diagnostic ignored "-Wpedantic"
using uint128_t = unsigned __int128;
#        pragma GCC diagnostic pop
#    endif
inline uint64_t umul128(uint64_t a, uint64_t b, uint64_t* high) noexcept {
    auto result = static_cast<uint128_t>(a) * static_cast<uint128_t>(b);
    *high = static_cast<uint64_t>(result >> 64U);
    return static_cast<uint64_t>(result);
}
#elif (defined(_WIN32) && SIZE_MAX == UINT64_MAX)
#    define HAS_UMUL128 1
#    include <intrin.h> // for __umulh
#    pragma intrinsic(__umulh)
#    ifndef _M_ARM64
#        pragma intrinsic(_umul128)
#    endif
inline uint64_t umul128(uint64_t a, uint64_t b, uint64_t* high) noexcept {
#    ifdef _M_ARM64
    *high = __umulh(a, b);
    return ((uint64_t)(a)) * (b);
#    else
    return _umul128(a, b, high);
#    endif
}
#else
#    define HAS_UMUL128 0
#endif

// copied this function from robinhood hash map
size_t hashBytes(void const* ptr, size_t const len) noexcept
{
    static constexpr uint64_t m = UINT64_C(0xc6a4a7935bd1e995);
    static constexpr uint64_t seed = UINT64_C(0xe17a1465);
    static constexpr unsigned int r = 47;

    auto data64 = reinterpret_cast<const uint64_t*>(ptr);
    uint64_t h = seed ^ (len * m);

    size_t const n_blocks = len / 8;
    for (size_t i = 0; i < n_blocks; ++i) {
        auto k = data64[i];

        k *= m;
        k ^= k >> r;
        k *= m;

        h ^= k;
        h *= m;
    }

    auto const data8 = reinterpret_cast<uint8_t const*>(data64 + n_blocks);
    switch (len & 7U) {
    case 7:
        h ^= static_cast<uint64_t>(data8[6]) << 48U;
        SWITCH_FALLTHROUGH;
    case 6:
        h ^= static_cast<uint64_t>(data8[5]) << 40U;
        SWITCH_FALLTHROUGH;
    case 5:
        h ^= static_cast<uint64_t>(data8[4]) << 32U;
        SWITCH_FALLTHROUGH;
    case 4:
        h ^= static_cast<uint64_t>(data8[3]) << 24U;
        SWITCH_FALLTHROUGH;
    case 3:
        h ^= static_cast<uint64_t>(data8[2]) << 16U;
        SWITCH_FALLTHROUGH;
    case 2:
        h ^= static_cast<uint64_t>(data8[1]) << 8U;
        SWITCH_FALLTHROUGH;
    case 1:
        h ^= static_cast<uint64_t>(data8[0]);
        h *= m;
        SWITCH_FALLTHROUGH;
    default:
        break;
    }

    h ^= h >> r;
    h *= m;
    h ^= h >> r;
    return static_cast<size_t>(h);
}

size_t hashInt(uint64_t obj) noexcept
{
#if HAS_UMUL128
    // 167079903232 masksum, 120428523 ops best: 0xde5fb9d2630458e9
    static constexpr uint64_t k = UINT64_C(0xde5fb9d2630458e9);
    uint64_t h;
    uint64_t l = umul128(obj, k, &h);
    return h + l;
#elif SIZE_MAX == UINT32_MAX
    uint64_t const r = obj * UINT64_C(0xca4bcaa75ec3f625);
    uint32_t h = static_cast<uint32_t>(r >> 32);
    uint32_t l = static_cast<uint32_t>(r);
    return h + l;
#else
    // murmurhash 3 finalizer
    uint64_t h = obj;
    h ^= h >> 33;
    h *= 0xff51afd7ed558ccd;
    h ^= h >> 33;
    h *= 0xc4ceb9fe1a85ec53;
    h ^= h >> 33;
    return static_cast<size_t>(h);
#endif
}


}
