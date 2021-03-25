#pragma once

#include <tl/int_types.hpp>
#include <initializer_list>
#include <assert.h>

namespace tl
{

template<typename T, size_t N>
class Array
{
public:
    static constexpr size_t s_size = N;
    typedef T ElemTypes;
    typedef const T* ConstIterator;
    typedef T* Iterator;

    constexpr size_t size()const{ return N; }

    T* data() { return _a; }
    const T* data()const { return _a; }

    const T& operator[](size_t i)const{ return _a[i]; }
    T& operator[](size_t i){ return _a[i]; }

    Iterator begin(){ return &_a[0]; }
    ConstIterator begin()const { return &_a[0]; }
    ConstIterator cbegin()const { return &_a[0]; }

    Iterator end() { return &_a[N]; }
    ConstIterator end()const { return &_a[N]; }
    ConstIterator cend()const { return &_a[N]; }

    bool operator==(const Array& o);
    bool operator<(const Array& o);

public:
    T _a[N];
};

template <typename T, size_t N>
bool Array<T, N>::operator==(const Array& o)
{
    for(size_t i=0; i<N; i++) {
        if(_a[i] != o._a[i])
            return false;
    }
    return true;
}

template <typename T, size_t N>
bool Array<T, N>::operator<(const Array& o)
{
    for(size_t i=0; i<N; i++) {
        if(_a[i] < o._a[i])
            return false;
    }
    return true;
}

}
