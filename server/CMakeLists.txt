cmake_minimum_required(VERSION 2.8)

project(FaceViewer)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

SET(GFRAME_SRC_DIR ${CMAKE_SOURCE_DIR}/renderer)
SET(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")

include_directories(${CMAKE_SOURCE_DIR}/client/include)
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS} ${GFRAME_SRC_DIR})

FILE(GLOB SERVER_SRC ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_executable(FaceViewer include/BlockingQueue.h ${SERVER_SRC} ${CMAKE_SOURCE_DIR}/client/src/Socket.cpp)

target_link_libraries(FaceViewer ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} GFrame)

INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/renderer/assets DESTINATION ${PROJECT_BINARY_DIR})
INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/renderer/shaders DESTINATION ${PROJECT_BINARY_DIR})
