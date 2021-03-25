#pragma once

#include "utils.hpp"

namespace reproject
{

void init();

void latlongToCubemap(
    u32& outTex, u32 outTexFaceSize,
    u32 inTex, glm::uvec2 inTexSize,
    u32 numSamples // if 0 will use direct method
);

void cubemapToLatlong(
    u32& outTex, uvec2 outTexSize,
    u32 inTex, u32 inTexFaceSize,
    u32 numSamples
);

}