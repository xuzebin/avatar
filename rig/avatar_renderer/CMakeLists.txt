cmake_minimum_required(VERSION 2.8)

project(avatar_renderer)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

SET(GFRAME_SRC_DIR ${PROJECT_SOURCE_DIR}/../src/rendering)
SET(GFRAME_STATIC_LIB ${PROJECT_SOURCE_DIR}/../libs/libGFrame.a)
SET(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")

include_directories(${PROJECT_SOURCE_DIR}/../include)
include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS} ${GFRAME_SRC_DIR})
include_directories(BlockingQueue.h)

add_executable(AvatarRenderer AvatarRenderer.cpp BlockingQueue.h ${PROJECT_SOURCE_DIR}/../src/Socket.cpp)

target_link_libraries(AvatarRenderer ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GFRAME_STATIC_LIB})

INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/../src/rendering/assets DESTINATION ${PROJECT_SOURCE_DIR}/build)
INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/../src/rendering/shaders DESTINATION ${PROJECT_SOURCE_DIR}/build)
