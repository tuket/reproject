#pragma once

#include "int_types.hpp"
#include <initializer_list>
#include <assert.h>

namespace tl
{

template <typename UIntType>
class BitsetT
{
    UIntType x;
public:
    BitsetT() : x(0) {}
    BitsetT(const BitsetT& o) : x(o.x) {}
    BitsetT(UIntType x) : x(x) {}
    BitsetT(std::initializer_list<bool> l) {
        assert(l.size() <= sizeof(x)*8);
        x = 0;
        for(UIntType i = 0; i < l.size(); i++)
            if(l.begin()[i])
                x |= ((UIntType)1 << i);
    }
    bool get(UIntType i)const { return x & ((UIntType)1 << i); }
    void set(UIntType i)const { x |= (UIntType)1 << i; }
    bool operator[](UIntType i)const { return get(i); }
    bool any()const { return x; }
    bool all()const { return x & (~(UIntType)0); }
    u16 count() const {
        u16 res = 0;
        for(u16 i = 0; i < sizeof(x)*8; i++)
            if(get(i))
                res++;
        return res;
    }
    operator UIntType()const { return x; }
    BitsetT operator|(BitsetT o)const { return x | o.x; }
    BitsetT operator&(BitsetT o)const { return x & o.x; }
    BitsetT operator^(BitsetT o)const { return x ^ o.x; }
    BitsetT operator|(UIntType o)const { return x | o; }
    BitsetT operator&(UIntType o)const { return x & o; }
    BitsetT operator^(UIntType o)const { return x ^ o; }
    BitsetT operator~()const { return ~x; }
};

typedef BitsetT<u8> Bitset8;
typedef BitsetT<u16> Bitset16;
typedef BitsetT<u32> Bitset32;
typedef BitsetT<u64> Bitset64;

/* TODO
template <u32 N>
class Bitset
{
*/



}
