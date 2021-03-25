project(stb)

add_library(stb STATIC stb_image.h stb_image_write.h stb.c)

target_include_directories(stb PUBLIC ${PROJECT_SOURCE_DIR})