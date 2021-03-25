#pragma once

#include <assert.h>
#include "int_types.hpp"

namespace tl
{

template <typename T>
class Span
{
public:
    Span();
    Span(T* data, size_t size);
    Span(T* from, T* to);
    template <size_t N>
    Span(T (&data)[N]);
    Span(Span& o);
    Span(const Span& o);
    operator T*() { return _data; }
    operator const T*()const { return _data; }
    operator Span<const T>()const { return {_data, _size}; }

    T& operator[](size_t i);
    const T& operator[](size_t i)const;

    T* begin() { return _data; }
    T* end() { return _data + _size; }
    const T* begin()const { return _data; }
    const T* end()const { return _data + _size;}
    size_t size()const { return _size; }

    Span<T> subArray(size_t from, size_t to);
    const Span<T> subArray(size_t from, size_t to)const; // "to" is not included i.e: [from, to)

private:
    T* _data;
    size_t _size;
};

// ---------------------------------------------------------------------------------------------
template <typename T>
Span<T>::Span()
    : _data(nullptr)
    , _size(0)
{}

template <typename T>
Span<T>::Span(T* data, size_t size)
    : _data(data)
    , _size(size)
{}

template <typename T>
Span<T>::Span(T* from, T* to)
    : _data(from)
    , _size(to - from)
{}

template <typename T>
template <size_t N>
Span<T>::Span(T (&data)[N])
    : _data(&data[0])
    , _size(N)
{}

template <typename T>
Span<T>::Span(Span<T>& o)
    : _data(o._data)
    , _size(o._size)
{}

template <typename T>
Span<T>::Span(const Span<T>& o)
    : _data(o._data)
    , _size(o._size)
{}

template <typename T>
T& Span<T>::operator[](size_t i) {
    assert(i < _size);
    return _data[i];
}

template <typename T>
const T& Span<T>::operator[](size_t i)const {
    assert(i < _size);
    return _data[i];
}

template <typename T>
Span<T> Span<T>::subArray(size_t from, size_t to) {
    assert(from <= to && to <= _size);
    return Span<T>(_data + from, _data + to);
}

template <typename T>
const Span<T> Span<T>::subArray(size_t from, size_t to)const {
    assert(from <= to && to <= _size);
    return Span<T>(_data + from, _data + to);
}

template <typename T>
using CSpan = Span<const T>;

}
