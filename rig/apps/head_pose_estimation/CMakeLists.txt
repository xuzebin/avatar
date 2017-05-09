cmake_minimum_required(VERSION 2.8)

project(HeadPose)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/client/include/)

add_executable(HeadPose HeadPoseEstimation.cpp)

target_link_libraries(HeadPose ${OpenCV_LIBS} dlib::dlib AvatarClient)

SET(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")

