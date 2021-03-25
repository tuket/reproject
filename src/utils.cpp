#include "utils.hpp"

#include <string.h>

char g_buffer[BUFFER_SIZE];

static void printShader(const char** srcs, int numSrcs)
{
    int line = 1;
    for(int srcInd = 0; srcInd < numSrcs; srcInd++) {
        const char* src = srcs[srcInd];
        const int n = strlen(src);
        int lineStart = 0;
        int i = 0;
        while(i < n) {
            while(i < n && src[i] != '\n')
                i++;
            
            printf("%3d | %.*s\n", line, i-lineStart, src+lineStart);
            lineStart = i+1;
            i++;
            line++;
        }
    }
}

const char* k_headerShadSrc =
R"GLSL(
#version 330 core

const float PI = 3.14159265358979;
const float INV_PI = 1.0 / PI;
)GLSL";

u32 makeShader(const char* src, u32 type)
{
    const char* srcs[2];
    srcs[0] = k_headerShadSrc;
    srcs[1] = src;
    const u32 shader = glCreateShader(type);
    glShaderSource(shader, 2, srcs, nullptr);
    glCompileShader(shader);
    i32 ok;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if(!ok) {
        glGetShaderInfoLog(shader, BUFFER_SIZE, nullptr, g_buffer);
        printf("%s\n", g_buffer);
        printShader(srcs, 2);
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

u32 makeProgram(const char* vertSrc, const char* fragSrc)
{
    const u32 vertShad = makeShader(vertSrc, GL_VERTEX_SHADER);
    if(vertShad == 0)
        return 0;

    const u32 fragShad = makeShader(fragSrc, GL_FRAGMENT_SHADER);
    if(fragShad == 0)
        return 0;

    const u32 prog = glCreateProgram();
    glAttachShader(prog, vertShad);
    glAttachShader(prog, fragShad);

    glLinkProgram(prog);

    i32 ok;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if(!ok) {
        glGetProgramInfoLog(prog, BUFFER_SIZE, nullptr, g_buffer);
        printf("Link Error:\n%s\n", g_buffer);
        glDetachShader(prog, vertShad);
        glDetachShader(prog, fragShad);
        glDeleteShader(vertShad);
        glDeleteShader(fragShad);
        return 0;
    }

    return prog;
}

u32 makeProgram(u32 vertShad, const char* fragSrc)
{
    const u32 fragShad = makeShader(fragSrc, GL_FRAGMENT_SHADER);
    if(fragShad == 0)
        return 0;

    const u32 prog = glCreateProgram();
    glAttachShader(prog, vertShad);
    glAttachShader(prog, fragShad);

    glLinkProgram(prog);

    i32 ok;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if(!ok) {
        glGetProgramInfoLog(prog, BUFFER_SIZE, nullptr, g_buffer);
        printf("Link Error:\n%s\n", g_buffer);
        glDetachShader(prog, fragShad);
        glDeleteShader(fragShad);
        return 0;
    }

    return prog;
}