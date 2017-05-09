cmake_minimum_required(VERSION 2.8)

project(AvatarServer)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

SET(GFRAME_SRC_DIR ${CMAKE_SOURCE_DIR}/renderer)
#SET(GFRAME_STATIC_LIB ${CMAKE_SOURCE_DIR}/libs/libGFrame.a)
SET(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")

include_directories(${CMAKE_SOURCE_DIR}/client/include)
include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS} ${GFRAME_SRC_DIR})

add_executable(AvatarServer AvatarServer.cpp ${CMAKE_SOURCE_DIR}/client/src/Socket.cpp)

#target_link_libraries(AvatarServer ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GFRAME_STATIC_LIB})
target_link_libraries(AvatarServer ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} GFrame)

INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/renderer/assets DESTINATION ${PROJECT_BINARY_DIR})
INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/renderer/shaders DESTINATION ${PROJECT_BINARY_DIR})
