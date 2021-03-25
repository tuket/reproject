/*

	Copyright (C) 2017 by Sergey A Kryukov: derived work
	http://www.SAKryukov.org
	http://www.codeproject.com/Members/SAKryukov

	Based on original work by Sergey Ryazanov:
	"The Impossibly Fast C++ Delegates", 18 Jul 2005
	https://www.codeproject.com/articles/11015/the-impossibly-fast-c-delegates

	MIT license:
	http://en.wikipedia.org/wiki/MIT_License

	Original publication: https://www.codeproject.com/Articles/1170503/The-Impossibly-Fast-Cplusplus-Delegates-Fixed
    
    This code has been modified by tuket
*/

#pragma once

#include <tl/move.hpp>

namespace tl
{

    template<typename T>
	class delegate_base;
	
	template<typename RET, typename ...PARAMS>
    class delegate_base<RET(PARAMS...)>
    {
	protected:
        using stub_type = RET(*)(void* this_ptr, PARAMS...);

		struct InvocationElement
		{
			InvocationElement() = default;
			
			InvocationElement(void* this_ptr, stub_type aStub)
			    : object(this_ptr), stub(aStub) {}
			
			void Clone(InvocationElement& target) const
			{
				target.stub = stub;
				target.object = object;
			}
			
			bool operator ==(const InvocationElement& another) const
			{
				return another.stub == stub && another.object == object;
			}
			
			bool operator !=(const InvocationElement& another) const
			{
				return another.stub != stub || another.object != object;
			}
			
			void* object = nullptr;
			stub_type stub = nullptr;
		};

	};


	template <typename T> class delegate;
	template <typename T> class multicast_delegate;

	template<typename RET, typename ...PARAMS>
	class delegate<RET(PARAMS...)> final : private delegate_base<RET(PARAMS...)>
	{
	public:

		delegate() = default;

		bool isNull() const { return invocation.stub == nullptr; }

        operator bool()const { return !isNull(); }
		
        bool operator ==(decltype(nullptr)) const {
			return this->isNull();
		}
        bool operator !=(decltype(nullptr)) const {
			return !this->isNull();
		}

		delegate(const delegate& another)
		    { another.invocation.Clone(invocation); }

		delegate& operator =(const delegate& another)
		{
			another.invocation.Clone(invocation);
			return *this;
		}

        delegate& operator =(decltype (nullptr))
        {
            invocation.object = nullptr;
            invocation.stub = nullptr;
        }

		bool operator == (const delegate& another) const
		    { return invocation == another.invocation; }
		bool operator != (const delegate& another) const
		    { return invocation != another.invocation; }

		bool operator ==(const multicast_delegate<RET(PARAMS...)>& another) const
		    { return another == (*this); }
		bool operator !=(const multicast_delegate<RET(PARAMS...)>& another) const
		    { return another != (*this); }

		template <class T, RET(T::*TMethod)(PARAMS...)>
		static delegate create(T* instance)
		{
			return delegate(instance, method_stub<T, TMethod>);
		}

		template <class T, RET(T::*TMethod)(PARAMS...) const>
		static delegate create(T const* instance)
		{
			return delegate(const_cast<T*>(instance), const_method_stub<T, TMethod>);
		}

		template <RET(*TMethod)(PARAMS...)>
		static delegate create()
		{
			return delegate(nullptr, function_stub<TMethod>);
		}

        template <typename... PARAMS_FN>
        RET operator()(PARAMS_FN&&... args) const
		{
            return (*invocation.stub)(invocation.object, tl::forward<PARAMS_FN>(args)...);
		}

	private:

		delegate(void* anObject, typename delegate_base<RET(PARAMS...)>::stub_type aStub)
		{
			invocation.object = anObject;
			invocation.stub = aStub;
		}

		void assign(void* anObject, typename delegate_base<RET(PARAMS...)>::stub_type aStub)
		{
			this->invocation.object = anObject;
			this->invocation.stub = aStub;
		}

        template <class T, RET(T::*TMethod)(PARAMS...)>
        static RET method_stub(void* this_ptr, PARAMS... params)
		{
			T* p = static_cast<T*>(this_ptr);
            return (p->*TMethod)(tl::forward<PARAMS>(params)...);
		}

        template <class T, RET(T::*TMethod)(PARAMS...) const>
        static RET const_method_stub(void* this_ptr, PARAMS... params)
		{
			T* const p = static_cast<T*>(this_ptr);
            return (p->*TMethod)(tl::forward<PARAMS>(params)...);
		}

        template <RET(*TMethod)(PARAMS...)>
        static RET function_stub(void* this_ptr, PARAMS... params)
		{
            return (TMethod)(tl::forward<PARAMS>(params)...);
		}

		friend class multicast_delegate<RET(PARAMS...)>;
		typename delegate_base<RET(PARAMS...)>::InvocationElement invocation;

	};

    template <typename Class, typename RetType, typename... Args>
    auto remove_class_from_member_fn(RetType(Class::*)(Args...))
        -> RetType(*)(Args...);

    template <typename Class, typename RetType, typename... Args>
    auto remove_class_from_member_fn(RetType(Class::*)(Args...)const)
        -> RetType(*)(Args...);

    template <typename Class, typename RetType, typename... Args>
    auto get_class_from_member_fn(RetType(Class::*)(Args...))
        -> Class;

    template <typename Class, typename RetType, typename... Args>
    auto get_class_from_member_fn(RetType(Class::*)(Args...)const)
        -> Class;

    template <typename T>
        struct remove_ptr { typedef T type; };
    template <typename T>
        struct remove_ptr<T*> { typedef T type; };

    template <typename MemberFn, MemberFn memberFn>
    auto _make_delegate(decltype(get_class_from_member_fn(memberFn))* obj)
        -> delegate<typename remove_ptr<decltype(remove_class_from_member_fn(memberFn))>::type>
    {
        return
            delegate<typename remove_ptr<decltype(remove_class_from_member_fn(memberFn))>::type>::template create<
                decltype(get_class_from_member_fn(memberFn)),
                memberFn
            >(obj);
    }

    template <typename PlainFn, PlainFn plainFn>
    auto _make_delegate() -> delegate<typename remove_ptr<PlainFn>::type>
    {
        return
            delegate<typename remove_ptr<PlainFn>::type>::template create<plainFn>();
    }

    #define make_delegate(obj, memberFn) _make_delegate<decltype(memberFn), memberFn>(obj)
    #define make_plain_delegate(plainFn) _make_delegate<decltype(plainFn), plainFn>()
}
