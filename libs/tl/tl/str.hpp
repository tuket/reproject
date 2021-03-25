#pragma once

#include <stdint.h>
#include "int_types.hpp"
#include <assert.h>

namespace tl
{

template <typename CharT>
class StrT;

// --- StrRef -------------------------------------------------------------------------------

template <typename CharT>
class CStrT
{
public:
    CStrT();
    CStrT(const CharT* str);
    CStrT(const StrT<CharT>& o);
    CStrT(const CharT* begin, const CharT* end);

    operator const CharT*()const { return _str; }
    const CharT* c_str()const{ return _str; }

    u32 size()const { return _size; }

    CharT operator[](u32 i)const {
        assert(i < _size);
        return _str[i];
    }

    const CharT* begin()const { return _str; }
    const CharT* end()const { return _str + _size; }

    bool operator==(CStrT o)const;
    bool operator==(const CharT* o)const;
    template<u32 N>
    bool operator==(const CharT (&o)[N])const { return *this == CStrT(o); }

private:
    u32 _size;
    const CharT* _str;
};

typedef CStrT<char> CStr;
typedef CStrT<char32_t> CStr32;

// --- Str ------------------------------------------------------------------------------

template <typename CharT>
class StrT
{
public:
    StrT();
    StrT(CStrT<CharT> o);
    StrT(const StrT& o);
    StrT(StrT&& o);
    StrT(const CharT* begin, const CharT* end);
    StrT(const CharT* cstr);
    StrT(const CharT* cstr, u32 size);
    StrT(u32 size, CharT c = '.');
    const StrT& operator=(CStrT<CharT> o);
    const StrT& operator=(const StrT& o);
    const StrT& operator=(StrT&& o);
    const StrT& operator=(const CharT* o);
    ~StrT();

    const CharT* c_str()const { return _str; }
    CharT* c_str(){ return _str; }

    u32 size()const { return _size; }

    const CharT& operator[](u32 i)const {
        assert(i < _size);
        return _str[i];
    }
    CharT& operator[](u32 i) {
        assert(i < _size);
        return _str[i];
    }

    const CharT* begin()const { return _str; }
    CharT* begin() { return _str; }
    const CharT* end()const { return _str + _size; }
    CharT* end() { return _str + _size; }

    bool operator==(CStrT<CharT> o)const;
    bool operator!=(CStrT<CharT> o)const { return !(*this == o); }

    StrT operator+(CStrT<CharT> o);
    StrT operator+(const CharT* o);
    StrT operator+(const StrT<CharT>& o);

    void operator+=(CStrT<CharT> o);
    void append(CStrT<CharT> o);
    void insert(u32 pos, CStrT<CharT> o);

    u32 capacity()const;

    void resize(u32 size);
    void resizeNoCopy(u32 size);

    void eraseRange(u32 fromIncluded, u32 toNotIncluded);

    void setStrAndSize(CharT* str, u32 size); // sets the internal values directly, use with caution

private:
    void construct(CStrT<CharT> o);
    void copy(CStrT<CharT> o);
    void stealMove(StrT&& o);
    CharT* growIfNeeded(u32 newSize);

    CharT* _str;
    u32 _size; // max 4GB

    static CharT _emptyStr[1];
    static u32 calcCapacity(u32 size);
};

template <typename T>
StrT<T> operator+(CStrT<T> a, CStrT<T> b);

template <typename T>
StrT<T> operator+(const T* a, CStrT<T> b)
    { return CStrT<T>(a) + b; }

template <typename T>
StrT<T> operator+(CStrT<T> a, const T* b)
    { return a + CStrT<T>(b); }

typedef StrT<char> Str;
typedef StrT<char32_t> Str32;

template <typename CharT>
u32 strlen(const CharT* str)noexcept;

// the following functions returns TOKEN_ERROR when an error is found
static const char32_t TOKEN_ERROR = 0xFFFFFFFF;
// given a utf8 encoded string get one utf32 token and advance to the next one
char32_t getUtf32TokenAndAdvance(const char*& ptr);
// given a utf8 string compute the equivalent utf32 string
Str32 toUtf32(CStr o);
Str toUtf8(CStr32 o);

}

using tl::Str;
using tl::Str32;
using tl::CStr;
using tl::CStr32;
