#include "str.hpp"

#include <tl/basic.hpp>
#include <tl/basic_math.hpp>
#include <tl/move.hpp>

namespace tl
{

template <typename CharT>
static CharT s_emptyStr[1] = {0};

template <typename CharT>
u32 strlen(const CharT* str)noexcept
{
    u32 len = 0;
    for(; *str; str++) len++;
    return len;
}
template u32 strlen(const char* str)noexcept;
template u32 strlen(const char32_t* str)noexcept;

static const u32 SMALLEST_CAPACITY = 32;

// --- CStr ------------------------------------------------------------------------------------------------

template<typename CharT>
CStrT<CharT>::CStrT()
    : CStrT(s_emptyStr<CharT>) {}

template<typename CharT>
CStrT<CharT>::CStrT(const CharT* str)
    : _size(strlen(str))
    , _str(str)
{}

template<typename CharT>
CStrT<CharT>::CStrT(const StrT<CharT>& o)
    : _size(o.size())
    , _str(o.c_str())
{}

template<typename CharT>
CStrT<CharT>::CStrT(const CharT* begin, const CharT* end)
    : _size(end - begin)
    , _str(begin)
{}

template<typename CharT>
bool CStrT<CharT>::operator==(CStrT o)const
{
    if(_size != o._size)
        return false;
    for(u32 i=0; i<_size; i++) {
        if(_str[i] != o._str[i])
            return false;
    }
    return true;
}

template<typename CharT>
bool CStrT<CharT>::operator==(const CharT* o)const
{
    const CharT* a = _str;
    const CharT* b = o;
    while(true) {
        if(*a != *b)
            return false;
        if(*a == 0)
            return true;
        a++;
        b++;
    }
}

template class CStrT<char>;
template class CStrT<char32_t>;

// --- Str ----------------------------------------------------------------------------------------------------------

template<typename CharT>
StrT<CharT>::StrT()
{
    _str = s_emptyStr<CharT>;
    _size = 0;
}

template<typename CharT>
StrT<CharT>::StrT(CStrT<CharT> o)
{
    construct(o);
}

template<typename CharT>
StrT<CharT>::StrT(const StrT& o)
{
    construct(o);
}

template<typename CharT>
StrT<CharT>::StrT(StrT&& o)
{
    stealMove(tl::move(o));
}

template<typename CharT>
StrT<CharT>::StrT(const CharT* begin, const CharT* end)
    : StrT(CStrT<CharT>(begin, end))
{}

template<typename CharT>
StrT<CharT>::StrT(const CharT* cstr)
    : StrT(CStrT<CharT>(cstr))
{}

template<typename CharT>
StrT<CharT>::StrT(const CharT* cstr, u32 size)
    : StrT()
{
    resizeNoCopy(size);
    for(u32 i=0; i<_size; i++) {
        _str[i] = cstr[i];
    }
}

template <typename CharT>
StrT<CharT>::StrT(u32 size, CharT c)
    : StrT()
{
    resizeNoCopy(size);
    for(u32 i=0; i<_size; i++) {
        _str[i] = c;
    }
}

template<typename CharT>
const StrT<CharT>& StrT<CharT>::operator=(CStrT<CharT> o)
{
    copy(o);
    return *this;
}

template<typename CharT>
const StrT<CharT>& StrT<CharT>::operator=(const StrT& o)
{
    copy(o);
    return *this;
}

template<typename CharT>
const StrT<CharT>& StrT<CharT>::operator=(StrT&& o)
{
    if(_str != s_emptyStr<CharT>)
        delete[] _str;
    stealMove(tl::move(o));
    return *this;
}

template<typename CharT>
const StrT<CharT>& StrT<CharT>::operator=(const CharT* o)
{
    *this = CStrT<CharT>(o);
    return *this;
}

template<typename CharT>
StrT<CharT>::~StrT()
{
    if(_str != s_emptyStr<CharT>)
        delete[] _str;
}

template<typename CharT>
bool StrT<CharT>::operator==(CStrT<CharT> o)const
{
    if(_size != o.size())
        return false;
    for(uint32_t i = 0; i < _size; i++) {
        if(o[i] != _str[i])
            return false;
    }
    return true;
}

template<typename CharT>
StrT<CharT> StrT<CharT>::operator+(CStrT<CharT> o)
{
    return CStrT<CharT>(*this) + o;
}

template<typename CharT>
StrT<CharT> StrT<CharT>::operator+(const CharT* o)
{
    return *this + CStrT<CharT>(o);
}

template<typename CharT>
StrT<CharT> StrT<CharT>::operator+(const StrT<CharT>& o)
{
    return *this + CStrT<CharT>(o);
}

template<typename CharT>
void StrT<CharT>::operator+=(CStrT<CharT> o)
{
    append(o);
}

template<typename CharT>
void StrT<CharT>::append(CStrT<CharT> o)
{
    const u32 newSize = _size + o.size();
    CharT *const newStr = growIfNeeded(newSize);
    if(newStr) {
        for(u32 i=0; i<_size; i++) {
            newStr[i] = _str[i];
        }
        if(_str != s_emptyStr<CharT>) {
            delete[] _str;
        }
        _str = newStr;
    }
    for(u32 i=0; i<o.size(); i++) {
        _str[_size+i] = o[i];
    }
    _str[newSize] = 0;
    _size = newSize;
}

template<typename CharT>
void StrT<CharT>::insert(u32 pos, CStrT<CharT> o)
{
    if(pos > size()) {
        assert(false);
        pos = size();
    }
    const u32 prevSize = size();
    const u32 numInserted = o.size();
    const u32 newSize = size() + o.size();
    resize(newSize);
    // shift to the right to make enough space for the text we need to insert
    for(u32 i=0; i < prevSize - pos; i++) {
        _str[newSize-i-1] = _str[prevSize-i-1];
    }
    // insert the string
    for(u32 i = 0; i < numInserted; i++) {
        _str[pos + i] = o[i];
    }
}

template<typename CharT>
u32 StrT<CharT>::capacity()const
{
    if(_str == s_emptyStr<CharT>)
        return 1;
    return calcCapacity(_size);
}

template<typename CharT>
void StrT<CharT>::resize(u32 size)
{
    if(size == 0) {
        _size = 0;
        _str[0] = 0;
        return;
    }
    if(auto str = growIfNeeded(size)) {
        for(u32 i = 0; i < _size; i++) {
            str[i] = _str[i];
        }
        if(_str != s_emptyStr<CharT>) {
            delete[] _str;
        }
        _str = str;
    }
    _size = size;
    _str[size] = 0;
}

template<typename CharT>
void StrT<CharT>::resizeNoCopy(u32 size)
{
    if(size == 0)
        return;
    if(auto str = growIfNeeded(size)) {
        if(_str != s_emptyStr<CharT>) {
            delete[] _str;
        }
        _str = str;
    }
    _size = size;
    _str[size] = 0;
}

template<typename CharT>
void StrT<CharT>::eraseRange(u32 from, u32 to)
{
    const u32 prevSize = _size;
    assert(from <= to && to <= _size);

    const u32 nToDel = to - from;
    for(u32 i = from; i + nToDel < prevSize; i++) {
        _str[i] = _str[i + nToDel];
    }
    resize(prevSize - nToDel);
}

template<typename CharT>
void StrT<CharT>::setStrAndSize(CharT* str, u32 size)
{
    _str = str;
    _size = size;
}

template<typename CharT>
void StrT<CharT>::construct(CStrT<CharT> o)
{
    _size = o.size();
    const u32 capacity = calcCapacity(_size);
    _str = new CharT[capacity];
    for(u32 i=0; i<_size; i++) {
        _str[i] = o[i];
    }
    _str[_size] = 0;
}

template<typename CharT>
void StrT<CharT>::copy(CStrT<CharT> o)
{
    if(_str != s_emptyStr<CharT>)
        delete[] _str;
    _size = o.size();
    const u32 capacity = calcCapacity(_size);
    _str = new CharT[capacity];
    for(u32 i=0; i<_size; i++) {
        _str[i] = o[i];
    }
    _str[_size] = 0;
}

template<typename CharT>
void StrT<CharT>::stealMove(StrT<CharT>&& o)
{
    _str = o._str;
    _size = o._size;
    o._str = s_emptyStr<CharT>;
    o._size = 0;
}

template<typename CharT>
CharT* StrT<CharT>::growIfNeeded(u32 newSize)
{
    const u32 neededCapacity = calcCapacity(newSize);
    if(capacity() < neededCapacity) {
        return new CharT[neededCapacity];
    }
    return nullptr;
}

template <typename CharT>
u32 StrT<CharT>::calcCapacity(u32 size)
{
    size++; //< null terminator
    const u32 allocSize = max<u32>(SMALLEST_CAPACITY, nextPowerOf2(size));
    return allocSize;
}

template <typename T>
StrT<T> operator+(CStrT<T> a, CStrT<T> b)
{
    StrT<T> res;
    res.resizeNoCopy(a.size() + b.size());
    for(u32 i = 0; i < a.size(); i++) {
        res[i] = a[i];
    }
    for(u32 i = 0; i < b.size(); i++) {
        res[a.size() + i] = b[i];
    }
    return res;
}
template StrT<char> operator+(CStrT<char> a, CStrT<char> b);
template StrT<char32_t> operator+(CStrT<char32_t> a, CStrT<char32_t> b);


template class StrT<char>;
template class StrT<char32_t>;

// --- functions ----------------------------------------------------------------------------------------------

char32_t getUtf32TokenAndAdvance(const char*& ptr)
{
    auto ptrU = reinterpret_cast<const unsigned char*>(ptr);
    static const char32_t TOKEN_ERROR = 0xFFFFFFFF;
    if((ptr[0] & 0b10000000) == 0) {  // 1 byte
        const char32_t res = static_cast<char32_t>(ptr[0]);
        ptr++;
        return res;
    }
    else if((ptr[0] & 0b11100000) == 0b11000000) { // 2 bytes
        char32_t res = static_cast<char32_t>(ptrU[0]);
        const char32_t x1 = static_cast<char32_t>(ptrU[1]);
        if((x1 & 0b11000000) != 0b10000000)
            return TOKEN_ERROR;
        res = (res & 0b00011111) << 6;
        res |= x1 & 0b00111111;
        ptr += 2;
        return  res;
    }
    else if((ptr[0] & 0b11110000) == 0b11100000) { // 3 bytes
        char32_t res = static_cast<char32_t>(ptrU[0]);
        const char32_t x1 = static_cast<char32_t>(ptrU[1]);
        if((x1 & 0b11000000) != 0b10000000)
            return TOKEN_ERROR;
        const char32_t x2 = static_cast<char32_t>(ptrU[2]);
        if((x2 & 0b11000000) != 0b10000000)
            return TOKEN_ERROR;
        res = (res & 0b00001111) << 12U;
        res |= (x1 & 0b00111111) << 6;
        res |= x2 & 0b00111111;
        ptr += 3;
        return  res;
    }
    else if((ptr[0] & 0b11111000) == 0b11110000) { // 4 bytes
        char32_t res = static_cast<char32_t>(ptrU[0]);
        const char32_t x1 = static_cast<char32_t>(ptrU[1]);
        if((x1 & 0b11000000) != 0b10000000)
            return TOKEN_ERROR;
        const char32_t x2 = static_cast<char32_t>(ptrU[2]);
        if((x2 & 0b11000000) != 0b10000000)
            return TOKEN_ERROR;
        const char32_t x3 = static_cast<char32_t>(ptrU[3]);
        if((x3 & 0b11000000) != 0b10000000)
            return TOKEN_ERROR;
        res = (res & 0b00000111) << 18U;
        res |= (x1 & 0b00111111) << 12U;
        res |= (x2 & 0b00111111) << 6U;
        res |= x3 & 0b00111111;
        ptr += 4;
        return  res;
    }
    return TOKEN_ERROR;
}

// compute the size of the string if converted to utf32
u32 calcSizeUtf32(CStr o)
{
    u32 size = 0;
    for(u32 i=0; i<o.size(); ) {
        if((o[i] & 0b10000000) == 0) { // 1 byte
            i++;
            size++;
        }
        else if((o[i] & 0b11100000) == 0b11000000) { // 2 bytes
            i += 2;
            size++;
        }
        else if((o[i] & 0b11110000) == 0b11100000) { // 3 bytes
            i += 3;
            size++;
        }
        else if((o[i] & 0b11111000) == 0b11110000) { // 4 bytes
            i += 4;
            size++;
        }
        else {
            return TOKEN_ERROR;
        }
    }
    return size;
}

Str32 toUtf32(CStr o)
{
    Str32 res;
    const u32 size = calcSizeUtf32(o);
    if(size == 0 || size == TOKEN_ERROR)
        return res;
    res.resizeNoCopy(size);
    const char* c = o.c_str();
    char32_t token = getUtf32TokenAndAdvance(c);
    for(u32 i = 0; i < size && token != TOKEN_ERROR; i++) {
        res[i] = token;
        token = getUtf32TokenAndAdvance(c);
    }
    return res;
}

u32 calcSizeUtf8(CStr32 str) // calc size if converted to utf8
{
    u32 size = 0;
    for(char32_t c : str) {
        if(c < 0x80)
            size += 1;
        else if(c < 0x800)
            size += 2;
        else if(c < 0x10000)
            size += 3;
        else
            size += 4;
    }
    return size;
}

Str toUtf8(CStr32 o)
{
    const u32 size = calcSizeUtf8(o);
    Str str(size);
    u32 j = 0;
    for(char32_t c : o) {
        if(c < 0x80) {
            str[j] = c & 0b0111'1111;
            j += 1;
        }
        else if(c < 0x800) {
            str[j+1] = 0b1000'0000 | (0b11'1111 & c);
            c >>= 6;
            str[j] = 0b1100'0000 | (0b1'1111 & c);
            j += 2;
        }
        else if(c < 0x10000) {
            str[j+2] = 0b1000'0000 | (0b11'1111 & c);
            c >>= 6;
            str[j+1] = 0b1000'0000 | (0b11'1111 & c);
            c >>= 6;
            str[j] = 0b1110'0000 | (0b1111 & c);
            j += 3;
        }
        else {
            str[j+3] = 0b1000'0000 | (0b11'1111 & c);
            c >>= 6;
            str[j+2] = 0b1000'0000 | (0b11'1111 & c);
            c >>= 6;
            str[j+1] = 0b1000'0000 | (0b11'1111 & c);
            c >>= 6;
            str[j] = 0b1111'0000 | (0b111 & c);
            j += 4;
        }
    }
    return str;
}

}
