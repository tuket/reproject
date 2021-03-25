#pragma once

#include <stdio.h>
#include <assert.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <tl/basic.hpp>
#include <tl/str.hpp>
#include <ctype.h>

extern GLFWwindow* g_window;

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::ivec2;
using glm::ivec3;
using glm::ivec4;
using glm::uvec2;
using glm::uvec3;
using glm::uvec4;
using glm::mat3;
using glm::mat4;

constexpr float PI = glm::pi<float>();

constexpr int BUFFER_SIZE = 16*1024;
extern char g_buffer[BUFFER_SIZE];

u32 makeShader(const char* src, u32 type);
u32 makeProgram(const char* vertSrc, const char* fragSrc);
u32 makeProgram(u32 vertShad, const char* fragSrc);

inline int strcmpi(const char* a, const char* b) {
    while(*a || *b) {
        if(*a < *b)
            return -1;
        else if(*a > *b)
            return +1;
        a++;
        b++;
    }
    return *b - *a;
}

inline bool toInt(int& res, CStr s)
{
    res = 0;
    for(char c : s) {
        if(c < '0' || c > '9')
            return false;
        res *= 10;
        res += c - '0';
    }
    return true;
}

inline bool parseResolution(uvec2& res, CStr s)
{
    res = {0, 0};
    int i = 0;
    int ri;
    for(ri = 0; ri < 2; ri++)
    {
        if(i >= s.size())
            return false;

        const char c = s[i];
        if(c >= '0' && c <= '9')
            res[ri] = c - '0';
        else
            return false;

        for(i++; i < s.size(); i++) {
            const char c = s[i];
            if(c >= '0' && c <= '9') {
                res[ri] *= 10;
                res[ri] += c - '0';
            }
            else if(c == 'x') {
                i++;
                break;
            }
            else
                return false;
        }
    }
    return true;
}

inline bool endsWithI(CStr str, CStr suffix)
{
    if(str.size() < suffix.size())
        return false;
    
    const int n = suffix.size();
    for(int i = 0; i < n; i++) {
        if(tolower(str[str.size() - n + i]) != tolower(suffix[i]))
            return false;
    }

    return true;
}

inline GLenum numChannelsToFormat(int nc)
{
    switch(nc)
    {
    case 1:
        return GL_RED;
    case 2:
        return GL_RG;
    case 3:
        return GL_RGB;
    case 4:
        return GL_RGBA;
    default:
        return 0;
    }
}

inline u8 getNumChannels(u32 format)
{
    switch(format)
    {
    case GL_RED:
        return 1;
    case GL_RG:
        return 2;
    case GL_RGB:
    case GL_BGR:
            return 3;
    case GL_RGBA:
    case GL_BGRA:
        return 4;
    // There are many more but I'm lazy
    }
    assert(false);
    return 0;
}

inline u8 getGetPixelSize(u32 format, u32 type)
{
    const u32 nc = getNumChannels(format);
    switch(type)
    {
    case GL_UNSIGNED_BYTE:
    case GL_BYTE:
        return nc;
    case GL_UNSIGNED_SHORT:
    case GL_SHORT:
    case GL_HALF_FLOAT:
        return 2*nc;
    case GL_UNSIGNED_INT:
    case GL_INT:
    case GL_FLOAT:
        return 4*nc;
    // there are many more but I'm lazy
    }
    assert(false);
    return 1;
}

inline GLenum formatTypeToInternalFormat(GLenum format, GLenum type)
{
    if(type == GL_UNSIGNED_BYTE)
    {
        switch(format)
        {
        case GL_RED: return GL_R8;
        case GL_RG: return GL_RG8;
        case GL_RGB: return GL_RGB8;
        case GL_RGBA: return GL_RGBA8;
        }
    }
    else if(type == GL_FLOAT)
    {
        switch(format)
        {
        case GL_RED: return GL_R32F;
        case GL_RG: return GL_RG32F;
        case GL_RGB: return GL_RGB32F;
        case GL_RGBA: return GL_RGBA32F;
        }
    }
    
    assert(false);
    return 0;
}