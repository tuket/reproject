#pragma once

#include <string.h>
#include <tl/basic_math.hpp>
#include <tl/move.hpp>
#include <assert.h>
#include <new>
#include <initializer_list>

namespace tl
{

template<typename T>
class Vector
{
public:
    typedef T* iterator;
    typedef const T* const_iterator;

    Vector()noexcept : _data(nullptr), _size(0), _capacity(0) {}
    Vector(const Vector& o)noexcept;
    Vector(Vector&& o)noexcept;
    Vector(size_t size)noexcept;
    Vector(size_t size, const T& val)noexcept;
    Vector(std::initializer_list<T> il);
    Vector& operator=(const Vector& o)noexcept;
    Vector& operator=(Vector&& o)noexcept;
    Vector& operator=(std::initializer_list<T> il);

    template<typename Iterator>
    Vector(Iterator begin, Iterator end);

    ~Vector();

    const T& operator[](size_t i)const noexcept{
        assert(i < _size);
        return _data[i];
    }
    T& operator[](size_t i)noexcept {
        assert(i < _size);
        return _data[i];
    }

    T* data() { return _data; }
    const T* data()const { return _data; }

    size_t size()const noexcept { return _size; }
    size_t capacity()const noexcept { return _capacity; }

    iterator begin()noexcept { return _data; }
    const_iterator begin()const noexcept { return _data; }
    iterator end()noexcept { return _data + _size; }
    const_iterator end()const noexcept { return _data + _size; }


    T& back()noexcept {
        assert(_size != 0);
        return _data[_size-1];
    }
    const T& back()const noexcept {
        assert(_size != 0);
        return _data[_size-1];
    }

    void push_back(const T& e)noexcept;
    void push_back(T&& e)noexcept;

    template<typename... Args>
    void emplace_back(Args&&... args)noexcept;

    void pop_back()noexcept;

    void reserve(size_t capacity)noexcept;
    template <typename... ConstructorArgs>
    void resize(size_t size, ConstructorArgs&&... args)noexcept;

    int findIndex(const T& x);
    template<typename Fn>
    int findIndexCond(const Fn& fn);

private:
    void grow(size_t capacity)noexcept;

    T* _data;
    size_t _size;
    size_t _capacity;
};

template <typename T>
Vector<T>::Vector(const Vector& o)noexcept
    : Vector()
{
    *this = o;
}

template <typename T>
Vector<T>::Vector(Vector&& o)noexcept
{
    *this = static_cast<Vector&&>(o);
}

template <typename T>
Vector<T>::Vector(size_t size)noexcept
    : Vector()
{
    resize(size);
}

template <typename T>
Vector<T>::Vector(size_t size, const T& val)noexcept
    : Vector()
{
    grow(size);
    for(size_t i = 0; i < size; i++) {
        new (&_data[i]) T(val);
    }
    _size = size;
}

template <typename T>
Vector<T>::Vector(std::initializer_list<T> il)
    : Vector()
{
    *this = il;
}

template <typename T>
Vector<T>& Vector<T>::operator=(const Vector& o)noexcept
{
    resize(0);
    _size = o._size;
    _capacity = o._capacity;
    grow(_capacity);
    for(size_t i=0; i<_size; i++) {
        new (&_data[i]) T(o[i]);
    }
    return *this;
}

template <typename T>
Vector<T>& Vector<T>::operator=(Vector&& o)noexcept
{
    _size = o._size;
    _capacity = o._capacity;
    _data = o._data;
    o._size = o._capacity = 0;
    o._data = nullptr;
    return *this;
}

template <typename T>
Vector<T>& Vector<T>::operator=(std::initializer_list<T> il)
{
    resize(0);
    grow(il.size());
    _size = il.size();
    auto it = il.begin();
    for(size_t i=0; i<_size; i++) {
        new (&_data[i]) T(*it);
        it++;
    }
}

template <typename T>
template<typename Iterator>
Vector<T>::Vector(Iterator begin, Iterator end)
    : Vector()
{
    for(auto it = begin; it != end; ++it) {
        push_back(*it);
    }
}

template <typename T>
Vector<T>::~Vector()
{
    for(size_t i = 0; i < _size; i++)
        _data[i].~T();
    if(_data)
        delete[] (char*)_data;
}

template <typename T>
void Vector<T>::push_back(const T& e)noexcept
{
    if(_size == _capacity)
        grow(_capacity+1);
    new ((void*)&_data[_size]) T(e);
    _size++;
}

template <typename T>
void Vector<T>::push_back(T&& e)noexcept
{
    if(_size == _capacity)
        grow(_capacity+1);
    new ((void*)&_data[_size]) T(tl::move(e));
    _size++;
}

template <typename T>
template <typename... Args>
void Vector<T>::emplace_back(Args&&... args)noexcept
{
    if(_size == _capacity)
        grow(_capacity+1);
    new ((void*)&_data[_size]) T(tl::forward<Args>(args)...);
    _size++;
}

template <typename T>
void Vector<T>::pop_back()noexcept
{
    assert(_size != 0);
    _size--;
    _data[_size].~T();
}

template <typename T>
void Vector<T>::reserve(size_t capacity)noexcept
{
    if(capacity > _capacity) {
        T* data = reinterpret_cast<T*>(new char[sizeof(T) * capacity]);
        if(_data) {
            memcpy(data, _data, _size * sizeof(T));
            delete[] (char*)_data;
        }
        _data = data;
        _capacity = capacity;
    }
}

template <typename T>
template <typename... ConstructorArgs>
void Vector<T>::resize(size_t size, ConstructorArgs&&... args)noexcept
{
    if(size > _capacity)
        grow(size);
    if(size < _size) {
        for(size_t i = size; i < _size; i++) {
            _data[i].~T();
        }
    }
    else if(size > _size) {
        for(size_t i = _size; i < size; i++) {
            new (&_data[i]) T(tl::forward<ConstructorArgs>(args)...);
        }
    }
    _size = size;
}

template <typename T>
int Vector<T>::findIndex(const T& x)
{
    for(int i = 0; i < _size; i++) {
        if(_data[i] == x)
            return i;
    }
    return -1;
}

template <typename T>
template <typename Fn>
int Vector<T>::findIndexCond(const Fn& fn)
{
    for(int i = 0; i < _size; i++) {
        if(fn(_data[i]))
            return i;
    }
    return -1;
}

template <typename T>
void Vector<T>::grow(size_t capacity)noexcept
{
    if(capacity <= 64) {
        reserve(64);
    }
    else if(capacity <= 256) {
        reserve(256);
    }
    else {
       const size_t n = nextPowerOf2(capacity);
       reserve(n);
    }
}

}
