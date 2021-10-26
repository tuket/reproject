#include "reproject.hpp"

namespace reproject
{

const char* k_vertShadSrc =
R"GLSL(
out vec2 v_tc;

void main()
{
    const vec2 tc[3] = vec2[3](
        vec2(0, 0),
        vec2(2, 0),
        vec2(0, 2)
    );
    v_tc = tc[gl_VertexID];
    gl_Position =  vec4(2 * v_tc - 1 , 0, 1);   
}
)GLSL";

const char* k_fragShadSrc_latlongToCubemap =
R"GLSL(
layout (location = 0) out vec4 o_color;
in vec2 v_tc;

uniform mat3 u_rayMtx;
uniform sampler2D u_tex;
uniform uint u_numSamples = 512u;
uniform float u_invFaceSize;

vec2 transformTc(vec2 tc)
{
    vec3 ray = u_rayMtx * vec3(2*tc - 1, +1);
    float r = length(ray.xz);
    float phi = atan(ray.x, ray.z);
    if(phi < 0)
        phi += 2*PI;
    float theta = atan(ray.y, r);
    return vec2(
        phi * 0.5 * INV_PI,
        0.5 + theta * INV_PI
    );
}

float RadicalInverse_VdC(uint bits) 
{
     bits = (bits << 16u) | (bits >> 16u);
     bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
     bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
     bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
     bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
     return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
vec2 Hammersley(uint i, uint N)
{
    return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}

void main()
{
    vec4 c = vec4(0);
    for(uint si = 0u; si < u_numSamples; si++) {
        vec2 tc = v_tc + (-0.5 + Hammersley(si, u_numSamples)) * u_invFaceSize;
        tc = transformTc(tc);
        c += texture(u_tex, tc);
    }
    o_color = c / u_numSamples;
}
)GLSL";

const char* k_fragShadSrc_cubemapToLatlong =
R"GLSL(
layout (location = 0) out vec4 o_color;
in vec2 v_tc;

uniform samplerCube u_tex;
uniform uint u_numSamples = 512u;
uniform vec2 u_invOutTexSize;

vec3 calcRay(vec2 tc)
{
    float phi = (-0.5 + tc.x) * 2 * PI;
    float theta = (-0.5 + tc.y) * PI;
    float r = cos(theta);
    return vec3(
        sin(phi) * r,
        sin(theta),
        cos(phi) * r
    );
}

float RadicalInverse_VdC(uint bits) 
{
     bits = (bits << 16u) | (bits >> 16u);
     bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
     bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
     bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
     bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
     return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
vec2 Hammersley(uint i, uint N)
{
    return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}

void main()
{
    vec4 c = vec4(0);
    for(uint si = 0u; si < u_numSamples; si++) {
        vec2 tc = vec2(v_tc.x, 1-v_tc.y); // invert Y so the output texture is not upside down
        tc += (-0.5 + Hammersley(si, u_numSamples)) * u_invOutTexSize; // jitter sample inside the pixels
        vec3 ray = calcRay(tc);
        c += texture(u_tex, ray);
    }
    o_color = c / u_numSamples;
}
)GLSL";

static u32 fbo;

struct LatlongToCubemapShad {
    u32 prog;
    struct {
        u32 tex;
        u32 rayMtx;
        u32 numSamples;
        u32 invFaceSize;
    } locs;
} latlongToCubemapShad;
static u32 latlongToCubemap_prog;

struct CubemapToLatlongShad {
    u32 prog;
    struct {
        u32 tex;
        u32 numSamples;
        u32 invOutTexSize;
    } locs;
} cubemapToLatlongShad;

static u32 sampler;

void init()
{
    glGenFramebuffers(1, &fbo);

    const u32 vertShad = makeShader(k_vertShadSrc, GL_VERTEX_SHADER);
    defer(glDeleteShader(vertShad));

    latlongToCubemapShad.prog = makeProgram(vertShad, k_fragShadSrc_latlongToCubemap);
    latlongToCubemapShad.locs.tex = glGetUniformLocation(latlongToCubemapShad.prog, "u_tex");
    latlongToCubemapShad.locs.rayMtx = glGetUniformLocation(latlongToCubemapShad.prog, "u_rayMtx");
    latlongToCubemapShad.locs.numSamples = glGetUniformLocation(latlongToCubemapShad.prog, "u_numSamples");
    latlongToCubemapShad.locs.invFaceSize = glGetUniformLocation(latlongToCubemapShad.prog, "u_invFaceSize");

    cubemapToLatlongShad.prog = makeProgram(vertShad, k_fragShadSrc_cubemapToLatlong);
    cubemapToLatlongShad.locs.tex = glGetUniformLocation(cubemapToLatlongShad.prog, "u_tex");
    cubemapToLatlongShad.locs.numSamples = glGetUniformLocation(cubemapToLatlongShad.prog, "u_numSamples");
    cubemapToLatlongShad.locs.invOutTexSize = glGetUniformLocation(cubemapToLatlongShad.prog, "u_invOutTexSize");

    glGenSamplers(1, &sampler);
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
}

void latlongToCubemap(
    u32& outTex, u32 outTexFaceSize,
    u32 inTex, uvec2 inTexSize,
    u32 numSamples // TODO: if 0 will use direct method
)
{
    const u32 faceSize = outTexFaceSize;
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &outTex);
    glBindTexture(GL_TEXTURE_2D, outTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4*faceSize, 3*faceSize, 0, GL_RGBA, GL_FLOAT, nullptr);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, outTex, 0);

    glUseProgram(latlongToCubemapShad.prog);
    glBindTexture(GL_TEXTURE_2D, inTex);
    glUniform1i(latlongToCubemapShad.locs.tex, 0);
    glBindSampler(0, sampler);
    glUniform1ui(latlongToCubemapShad.locs.numSamples, numSamples);
    glUniform1f(latlongToCubemapShad.locs.invFaceSize, 1.f / faceSize);

    static const mat3 rayMatrices[] = {
        {{0, 0, +1}, {0, +1, 0}, {-1, 0, 0}}, // right
        {{0, 0, -1}, {0, +1, 0}, {+1, 0, 0}}, // left
        {{+1, 0, 0}, {0, 0, +1}, {0, -1, 0}}, // up
        {{+1, 0, 0}, {0, 0, -1}, {0, +1, 0}}, // down
        {{+1, 0, 0}, {0, +1, 0}, {0, 0, -1}}, // front
        {{-1, 0, 0}, {0, +1, 0}, {0, 0, +1}}, // back
    };
    const ivec2 texRegions[] = {
        {2*faceSize, outTexFaceSize}, // right
        {0, faceSize}, // left
        {faceSize, 2*faceSize}, // up
        {faceSize, 0}, // down
        {faceSize, faceSize}, // front
        {3*faceSize, faceSize}, // back
    };

    glViewport(0, 0, 4*faceSize, 3*faceSize);
    glScissor(0, 0, 4*faceSize, 3*faceSize);
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);

    for(int f = 0; f < 6; f++) {
        const int rx = texRegions[f].x;
        const int ry = texRegions[f].y;
        glViewport(rx, ry, faceSize, faceSize);
        glScissor(rx, ry, faceSize, faceSize);

        glUniformMatrix3fv(latlongToCubemapShad.locs.rayMtx, 1, GL_FALSE, &rayMatrices[f][0][0]);

        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

}

void cubemapToLatlong(
    u32& outTex, uvec2 outTexSize,
    u32 inTex, u32 inTexFaceSize,
    u32 numSamples // TODO: if 0 will use direct method. It should average the pixels that fall inside the projected pixel's quad
)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &outTex);
    glBindTexture(GL_TEXTURE_2D, outTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, outTexSize.x, outTexSize.y, 0, GL_RGBA, GL_FLOAT, nullptr);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, outTex, 0);

    glUseProgram(cubemapToLatlongShad.prog);
    glBindTexture(GL_TEXTURE_CUBE_MAP, inTex);
    glUniform1i(cubemapToLatlongShad.locs.tex, 0);
    glBindSampler(0, sampler);
    glUniform1ui(cubemapToLatlongShad.locs.numSamples, numSamples);
    const vec2 invOutTexSize = vec2(1.f) / vec2(outTexSize);
    glUniform2f(cubemapToLatlongShad.locs.invOutTexSize, invOutTexSize.x, invOutTexSize.y);

    glViewport(0, 0, outTexSize.x, outTexSize.y);
    glScissor(0, 0, outTexSize.x, outTexSize.y);

    glDrawArrays(GL_TRIANGLES, 0, 3);
}

}