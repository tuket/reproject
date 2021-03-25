#include "random.hpp"

#include <assert.h>
#include <time.h>

namespace tl
{

#ifndef NDEBUG
static bool defaultRngInitialized = false;
#endif

void randSeed() {
#ifndef NDEBUG
    defaultRngInitialized = true;
#endif
    const time_t t = time(nullptr);
    pcg32_srandom(423432*t, 6236347*t);
}

void randSeed(RandomGenerator32* rndGen) {

}

i32 randI32(i32 end)
{
#ifndef NDEBUG
    assert(defaultRngInitialized);
#endif
    assert(end >= 0);
    return pcg32_boundedrand(end + 1);
}

i32 randI32(i32 begin, i32 end)
{
    assert(begin <= end);
    return begin + pcg32_boundedrand(end - begin + 1);
}

}
