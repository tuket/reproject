#pragma once
// unique pointer

#include "move.hpp"

typedef decltype(nullptr) nullptr_t;

namespace tl
{

template <typename T>
class uptr
{
public:
    uptr(T* ptr = nullptr) noexcept
        : _ptr(ptr)
    {}

    template<typename T2>
    uptr(uptr<T2>&& o) noexcept
    {
        _ptr = o.release();
    }

    template<typename T2>
    void operator=(uptr<T2>&& o) noexcept
    {
        reset();
        _ptr = o.release();
    }

    template <typename T2>
    void operator=(T2* o) noexcept
    {
        reset();
        _ptr = o;
    }

    T* get() const noexcept
    { 
        return _ptr;
    }

    void reset(T* ptr = nullptr) noexcept
    {
        if(_ptr != nullptr)
            delete _ptr;
        _ptr = ptr;
    }

    T* release() noexcept
    {
        T* ptr = _ptr;
        _ptr = nullptr;
        return ptr;
    }

    T* operator->() const noexcept
    {
        return _ptr;
    }

    T& operator*() const noexcept
    {
        return *_ptr;
    }

    operator bool() const noexcept
    {
        return _ptr != nullptr;
    }

    bool operator==(nullptr_t) const noexcept
    {
        return _ptr == nullptr;
    }

    bool operator!=(nullptr_t) const noexcept
    {
        return _ptr != nullptr;
    }

    ~uptr()
    {
        delete _ptr;
    }

private:
    T* _ptr;
};



template<typename T, typename... Args>
uptr<T> make_unique(Args&&... args)
{
    return uptr<T>(new T(forward<Args>(args)...));
}

template<typename To, typename From>
To* dcast(const uptr<From>& p)
{
    return dynamic_cast<To*>(p.get());
}

}
