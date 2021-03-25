project(glad)

add_library(glad STATIC include/glad/glad.h src/glad.c)

target_include_directories(glad PUBLIC include)
target_link_libraries(glad ${CMAKE_DL_LIBS})