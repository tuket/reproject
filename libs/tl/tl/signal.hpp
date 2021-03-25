#pragma once

#include <tl/containers/vector.hpp>
#include <tl/delegate.hpp>

namespace tl
{

template <typename... Ts>
class Signal
{
public:
    typedef tl::delegate<void(Ts...)> Callback;

    class SubscriptionData
    {
        friend class Subscription;
        friend class Signal;
        SubscriptionData(const Signal* signal, const Callback& callback)
            : callback(callback), signal(signal) {}
        Callback callback;
        const Signal* signal;
    };

    class Subscription
    {
        friend class Signal;
    public:
        Subscription()
            : data(nullptr) {}
        Subscription(Subscription&& o)
        {
            data = o.data;
            o.data = nullptr;
        }
        void operator=(Subscription&& o)
        {
            data = o.data;
            o.data = nullptr;
        }
        void cancel()
        {
            if(data) {
                data->signal->onSubscriptionDeleted(*this);
                data = nullptr;
            }
        }
        ~Subscription()
        {
            cancel();
        }
    protected:
        Subscription(SubscriptionData* data)
            : data(data) {}
        SubscriptionData* data;
    };

    Signal()
    {

    }

    [[nodiscard]]
    Subscription subscribe(const Callback& callback)const
    {
        SubscriptionData* s = new SubscriptionData(this, callback);
        subscriptions.push_back(s);
        return Subscription(s);
    }

    void broadcast(Ts... ts)
    {
        for(auto subscription : subscriptions)
            subscription->callback(ts...);
    }

    ~Signal()
    {
        for(auto subscription : subscriptions)
            delete subscription;
    }

private:
    void onSubscriptionDeleted(Subscription& subscription)const 
    {
        for(unsigned i=0; i<subscriptions.size(); i++)
        {
            if(subscription.data == subscriptions[i])
            {
                delete subscriptions[i];
                subscriptions[i] = subscriptions.back();
                subscriptions.pop_back();
                break;
            }
        }
    }

    mutable tl::Vector<SubscriptionData*> subscriptions;
};

}
