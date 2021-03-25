cmake_minimum_required(VERSION 3.8)
project(reproject)

set (CMAKE_CXX_STANDARD 17)

option(FINAL_RELEASE "Final Release" OFF)
if(FINAL_RELEASE)
    add_definitions(-D_FINAL)
endif()

# this function preppends a path to all files in a list
FUNCTION(PREPEND var prefix)
SET(listVar "")
   FOREACH(f ${ARGN})
   LIST(APPEND listVar "${prefix}/${f}")
   ENDFOREACH(f)
   SET(${var} "${listVar}" PARENT_SCOPE)
ENDFUNCTION(PREPEND)

add_subdirectory(libs/glad)
add_subdirectory(libs/glfw)
add_subdirectory(libs/glm)
add_subdirectory(libs/tl)
add_subdirectory(libs/stb)

set(SOURCES
    main.cpp
    utils.hpp utils.cpp
    reproject.cpp
)
PREPEND(SOURCES "src/" ${SOURCES})

add_executable(reproject
    ${SOURCES}
)

target_link_libraries(reproject
    glad
    glfw
    glm
    tl
    stb
)