#pragma once

#include <glm/vec2.hpp>

namespace tl
{

template <typename T, typename TV, typename TB>
struct RectCommonStuff
{
    TV pMin;
    TV pMax;
    TB& xMin = pMin.x;
    TB& yMin = pMin.y;
    TB& xMax = pMax.x;
    TB& yMax = pMax.y;
    
    RectCommonStuff() {}
    RectCommonStuff(const RectCommonStuff& o) = default;
    RectCommonStuff(TB xMin, TB yMin, TB xMax, TB yMax)
        : pMin(xMin, yMin), pMax(xMax, yMax) {}
    RectCommonStuff(TB w, TB h)
        : pMin(0, 0), pMax(w, h) {}
    RectCommonStuff(const TV& size)
        : pMin(0, 0), pMax(size) {}
    RectCommonStuff(const TV& pMin, const TV& pMax)
        : pMin(pMin), pMax(pMax) {}

    const T& operator=(const T& o)
    {
        pMin = o.pMin;
        pMax = o.pMax;
        return *static_cast<T*>(this);
    }

    bool operator==(const T& o)const
    {
        return pMin == o.pMin && pMax == o.pMax;
    }

    bool operator!=(const T& o)const
    {
        return pMin != o.pMin || pMax != o.pMax;
    }
};

struct rect : RectCommonStuff<rect, glm::vec2, float>
{
    rect() {}
    rect(const rect& o) : RectCommonStuff(o) {}
    rect(float xMin, float yMin, float xMax, float yMax) : RectCommonStuff(xMin, yMin, xMax, yMax) {}
    rect(const glm::vec2& pMin, const glm::vec2& pMax) : RectCommonStuff(pMin, pMax) {}
    rect(float w, float h) : RectCommonStuff(w, h) {}
    rect(const glm::vec2& size) : RectCommonStuff(size) {}
    const rect& operator=(const rect& o) { return RectCommonStuff::operator=(o); }
};

struct irect : RectCommonStuff<irect, glm::ivec2, int>
{
    irect() {}
    irect(const irect& o) : RectCommonStuff(o) {}
    irect(int xMin, int yMin, int xMax, int yMax) : RectCommonStuff(xMin, yMin, xMin, xMax) {}
    irect(const glm::ivec2& pMin, const glm::ivec2& pMax) : RectCommonStuff(pMin, pMax) {}
    irect(int w, int h) : RectCommonStuff(w, h) {}
    irect(const glm::ivec2& size) : RectCommonStuff(size) {}
    const irect& operator=(const irect& o) { return RectCommonStuff::operator=(o); }
};

struct urect : RectCommonStuff<urect, glm::uvec2, unsigned>
{
    urect() {}
    urect(const urect& o) : RectCommonStuff(o) {}
    urect(unsigned xMin, unsigned yMin, unsigned xMax, unsigned yMax) : RectCommonStuff(xMin, yMin, xMin, xMax) {}
    urect(const glm::uvec2& pMin, const glm::uvec2& pMax) : RectCommonStuff(pMin, pMax) {}
    urect(unsigned w, unsigned h) : RectCommonStuff(w, h) {}
    urect(const glm::uvec2& size) : RectCommonStuff(size) {}
    const urect& operator=(const urect& o) { return RectCommonStuff::operator=(o); }
};

static rect toFloat(const irect& r)
{
    return rect{(float)r.xMin, (float)r.yMin, (float)r.xMax, (float)r.yMax};
}

}
