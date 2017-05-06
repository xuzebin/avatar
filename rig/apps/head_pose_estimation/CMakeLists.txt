cmake_minimum_required(VERSION 2.8)

project(HeadPose)

find_package(OpenCV REQUIRED)

include(../../libs/dlib/cmake)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/../../include/)

file(GLOB PROJECT_SOURCES ${PROJECT_SOURCE_DIR}/../../src/*.cpp)

add_executable(HeadPose HeadPoseEstimation.cpp ${PROJECT_SOURCES})

target_link_libraries(HeadPose ${OpenCV_LIBS} dlib)

SET(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")

