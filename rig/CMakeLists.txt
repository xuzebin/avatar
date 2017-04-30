cmake_minimum_required(VERSION 2.8)

project(avatar)

find_package(OpenCV REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

include(./libs/dlib/cmake)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/include/)
include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/src/rendering)

file(GLOB_RECURSE PROJECT_SOURCES src/*.cpp)
file(GLOB_RECURSE PROJECT_HEADERS include/*.h)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS})

target_link_libraries(${PROJECT_NAME} dlib::dlib ${OpenCV_LIBS} ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GFRAME_STATIC_LIB})

SET(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")

#SET(GFRAME_STATIC_LIB ${PROJECT_SOURCE_DIR}/libs/libGFrame.a)
#add_subdirectory(render)
#add_subdirectory(src/rendering_examples)


#INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/src/rendering/assets DESTINATION ${PROJECT_SOURCE_DIR}/build4)
#INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/src/rendering/shaders DESTINATION ${PROJECT_SOURCE_DIR}/build4)

#add_subdirectory(avatar_server)
#add_subdirectory(src/rendering)
