#pragma once

namespace tl
{

template<class InputIt, class OutputIt>
OutputIt copy(InputIt begin, InputIt end, 
              OutputIt destBegin)
{
    while (begin != end) {
        *destBegin++ = *begin++;
    }
    return destBegin;
}

}