#pragma once

namespace tl
{

template< class ForwardIt, class T >
void fill(ForwardIt begin, ForwardIt end, const T& value)
{
    for (; begin != end; ++begin) {
        *begin = value;
    }
}

template <typename Container, typename T>
void fill(Container& c, const T& value)
{
    fill(c.begin(), c.end(), value);
}

}