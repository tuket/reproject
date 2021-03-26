#include "utils.hpp"
#include "reproject.hpp"
#include <tl/str.hpp>
#include <ctype.h>
#include <stb_image.h>
#include <stb_image_write.h>

GLFWwindow* g_window;

static const char* geGlErrStr(GLenum const err)
{
    switch (err) {
    case GL_NO_ERROR: return "GL_NO_ERROR";
    case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
    case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
    case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
    case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
    case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
#ifdef GL_STACK_UNDERFLOW
    case GL_STACK_UNDERFLOW: return "GL_STACK_UNDERFLOW";
#endif
#ifdef GL_STACK_OVERFLOW
    case GL_STACK_OVERFLOW: return "GL_STACK_OVERFLOW";
#endif
    default:
        assert(!"unknown error");
        return nullptr;
    }
}

static void glErrorCallback(const char *name, void *funcptr, int len_args, ...) {
    GLenum error_code;
    error_code = glad_glGetError();
    if (error_code != GL_NO_ERROR) {
        fprintf(stderr, "ERROR %s in %s\n", geGlErrStr(error_code), name);
        assert(false);
    }
}

static void* loadImage(CStr filePath, int& w, int& h, GLenum& format, GLenum& type)
{
    int nc;
    void* data;
    if(endsWithI(filePath, ".hdr")) {
        data = stbi_loadf(filePath.begin(), &w, &h, &nc, 0);        
        type = GL_FLOAT;
    }
    else {
        data = stbi_load(filePath.begin(), &w, &h, &nc, 0);
        type = GL_UNSIGNED_BYTE;
    }

    format = numChannelsToFormat(nc);
    if(format == 0)
        return nullptr;
    return data;
}

static void saveImageBoundTex(CStr filePath, int w, int h, int nc, GLenum format, void* data)
{
    bool ok = false;
    if(endsWithI(filePath, ".hdr")) {
        glReadPixels(0, 0, w, h, format, GL_FLOAT, data);
        ok = stbi_write_hdr(filePath, w, h, nc, (float*)data);
    }
    else {
        glReadPixels(0, 0, w, h, format, GL_UNSIGNED_BYTE, data);
        if(endsWithI(filePath, ".png"))
            ok = stbi_write_png(filePath, w, h, nc, data, w*nc);
        else if(endsWithI(filePath, ".bmp"))
            ok = stbi_write_bmp(filePath, w, h, nc, data);
        else if(endsWithI(filePath, ".tga"))
            ok = stbi_write_tga(filePath, w, h, nc, data);
        else if(endsWithI(filePath, ".jpg") || endsWithI(filePath, ".jpeg"))
            ok = stbi_write_jpg(filePath, w, h, nc, data, 99);
    }

    if(!ok)
        printf("Error saving image: %s\n", filePath.c_str());
}

static void saveImageBoundTex(CStr filePath, int w, int h, int nc, void* data)
    { saveImageBoundTex(filePath, w, h, nc, numChannelsToFormat(nc), data); }

void command_latlongToCubemap(const char* inFilePath, const char* outFilePath, int faceSize, u32 numSamples)
{
    GLenum format, type;
    int w, h, nc;
    void *inData = loadImage(inFilePath, w, h, format, type);
    if(!inData || !format) {
        printf("Couldn't read input file '%s'\n", inFilePath);
        //printHelp();
        return;
    }
    nc = getNumChannels(format);
    const GLenum internalFormat = formatTypeToInternalFormat(format, GL_FLOAT);

    u32 inTex;
    glGenTextures(1, &inTex);
    glBindTexture(GL_TEXTURE_2D, inTex);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, type, inData);

    u32 outTex;
    reproject::latlongToCubemap(outTex, faceSize, inTex, {w, h}, numSamples);
    
    u8* outData;
    if(type == GL_UNSIGNED_BYTE)
        outData = new u8[nc * 4*3 * faceSize*faceSize];
    else if (type == GL_FLOAT)
        outData = new u8[sizeof(float) * nc *4*3 * faceSize*faceSize];
    else
        assert(false);
    defer(delete[] outData);

    glFinish();
    glBindTexture(GL_TEXTURE_2D, outTex);

    saveImageBoundTex(outFilePath, 4*faceSize, 3*faceSize, nc, format, outData);
}

static void uploadCubemapTexture(u32 mipLevel, u32 w, u32 h, u32 internalFormat, u32 dataFormat, u32 dataType, u8* data)
{
    const u8 ps = getGetPixelSize(dataFormat, dataType);
    const u32 side = w / 4;
    assert(3*side == h);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, w);
    defer(glPixelStorei(GL_UNPACK_ROW_LENGTH, 0));
    auto upload = [&](GLenum face, u32 offset) {
        glTexImage2D(face, mipLevel, internalFormat, side, side, 0, dataFormat, dataType, data + offset);
    };
    upload(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, ps * w*side);
    upload(GL_TEXTURE_CUBE_MAP_POSITIVE_X, ps * (w*side + 2*side));
    upload(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, ps * (w*2*side + side));
    upload(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, ps * side);
    upload(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, ps * (w*side + 3*side));
    upload(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, ps * (w*side + side));
}

static void uploadCubemapAsArrayTexture(u32 w, u32 h, u32 internalFormat, u32 dataFormat, u32 dataType, u8* data)
{
    const u32 side = w / 4;
    assert(3*side == h);
    const u8 ps = getGetPixelSize(dataFormat, dataType);

    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, internalFormat, side, side, 6, 0, dataFormat, dataType, nullptr);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, w);
    defer(glPixelStorei(GL_UNPACK_ROW_LENGTH, 0));
    auto upload = [&](int face, u32 offset) {
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0,
            0, 0, face,
            side, side, 1,
            dataFormat, dataType, data + offset
        );
    };
    upload(0, ps * w*side);
    upload(1, ps * (w*side + 2*side));
    upload(2, ps * (w*2*side + side));
    upload(3, ps * side);
    upload(4, ps * (w*side + 3*side));
    upload(5, ps * (w*side + side));
}

void command_cubemapToLatlong(const char* inFilePath, const char* outFilePath, ivec2 outTexSize, u32 numSamples)
{
    GLenum format, type;
    int w, h, nc;
    void *inData = loadImage(inFilePath, w, h, format, type);
    if(!inData || !format) {
        printf("Couldn't read input file '%s'\n", inFilePath);
        //printHelp();
        return;
    }
    nc = getNumChannels(format);
    const GLenum internalFormat = formatTypeToInternalFormat(format, GL_FLOAT);
    const int faceSize = w / 4;

    u32 inTex;
    glGenTextures(1, &inTex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, inTex);
    uploadCubemapTexture(0, w, h, internalFormat, format, type, (u8*)inData);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    u32 outTex;
    reproject::cubemapToLatlong(outTex, outTexSize, inTex, faceSize, numSamples);
    
    u8* outData;
    if(type == GL_UNSIGNED_BYTE)
        outData = new u8[nc * outTexSize.x * outTexSize.y];
    else if (type == GL_FLOAT)
        outData = new u8[sizeof(float) * nc * outTexSize.x * outTexSize.y];
    else
        assert(false);
    defer(delete[] outData);

    glFinish();
    glBindTexture(GL_TEXTURE_2D, outTex);

    saveImageBoundTex(outFilePath, outTexSize.x, outTexSize.y, nc, format, outData);
}

void demo_proj();
void demo_latlongToCubemap();

void printHelp()
{
    printf(
        "Usage:\n"
        "reproject latlongToCubemap <inputFile> <outputFile> <outputFaceResolution> [numSamples]\n"
        "reproject cubemapToLatlong <inputFile> <outputFile> <outputWidth>x<outputHeight> [numSamples]\n"
    );
}

int main(int argc, char** argv)
{
    glfwSetErrorCallback(+[](int error, const char* description) {
        fprintf(stderr, "Glfw Error %d: %s\n", error, description);
    });
    if (!glfwInit()) {
        fprintf(stderr, "error glfwInit\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 8);

    g_window = glfwCreateWindow(200, 100, "reproject", nullptr, nullptr);
    if (g_window == nullptr) {
        fprintf(stderr, "error creating the window\n");
        return -1;
    }

    glfwMakeContextCurrent(g_window);
    glfwSwapInterval(0); // Enable vsync

    if (gladLoadGL() == 0) {
        fprintf(stderr, "error in gladLoadGL()\n");
        return 3;
    }
    glad_set_post_callback(glErrorCallback);

    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    {
        // OpenGL 3.3+ requires a bound VAO for drawing
        // When drawing a full-screen quad we don't need a vao.
        // So here we bind a dummy vao, just in case the first thing we will draw is a full-screen quad
        u32 dummyVao;
        glGenVertexArrays(1, &dummyVao);
        glBindVertexArray(dummyVao);
    }
    reproject::init();

    glfwHideWindow(g_window);

    if(argc != 5 && argc != 6) {
        printf("Invalid number of params\n");
        printHelp();
        return -1;
    }

    if(strcmp_ci(argv[1], "latlongToCubemap") == 0)
    {
        const char* inFilePath = argv[2];
        const char* outFilePath = argv[3];
        
        int faceSize;
        if(!toInt(faceSize, argv[4]) || faceSize == 0) {
            printf("The 'outputFaceResolution' parameter must be a positive integer\n");
            printHelp();
            return -1;
        }

        int numSamples = 128;
        if(argc == 6) {
            if(!toInt(numSamples, argv[5])) {
                printf("The 'numSamples' parameter must be an interger\n");
                printHelp();
                return -1;
            }
        }

        command_latlongToCubemap(inFilePath, outFilePath, faceSize, numSamples);
    }
    else if(strcmp_ci(argv[1], "cubemapToLatlong") == 0)
    {
        const char* inFilePath = argv[2];
        const char* outFilePath = argv[3];

        uvec2 outTexSize;
        if(!parseResolution(outTexSize, argv[4])) {
            printf("Wrong format for output resolution\n");
            printHelp();
            return -1;
        }
        
        int numSamples = 128;
        if(argc == 6) {
            if(!toInt(numSamples, argv[5])) {
                printf("The 'numSamples' parameter must be an interger\n");
                printHelp();
                return -1;
            }
        }

        command_cubemapToLatlong(inFilePath, outFilePath, outTexSize, numSamples);
    }
    else {
        printf("Invalid command\n");
        printHelp();
    }

    //glfwSwapBuffers(g_window); // uncomment in order to be able to debug with renderdoc
}