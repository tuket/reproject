#pragma once

#include "pcg_basic.h"
#include <tl/int_types.hpp>

namespace tl
{
    typedef pcg32_random_t RandomGenerator32;

    void randSeed();
    //oid initSeed(RandomGenerator32* rndGen);

    i32 randI32(i32 end); // [0, end]
    i32 randI32(i32 begin, i32 end); // [begin, end]
    //i32 randI32(RandomGenerator32* rndGen);
}
