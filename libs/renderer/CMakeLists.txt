cmake_minimum_required(VERSION 2.8)

project(GFrame)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

file(GLOB_RECURSE SRC_FILES *.cpp)

include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})

add_library(GFrame STATIC ${SRC_FILES})

SET(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")
