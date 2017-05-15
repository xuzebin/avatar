cmake_minimum_required(VERSION 2.8)

project(HeadPoseEstimation)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/client/include/)

add_executable(HeadPoseEstimation HeadPoseEstimation.cpp)

target_link_libraries(HeadPoseEstimation ${OpenCV_LIBS} dlib::dlib AvatarClient)

SET(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")

INSTALL(FILES ${CMAKE_SOURCE_DIR}/data/shape_predictor_68_face_landmarks.dat DESTINATION ${CMAKE_BINARY_DIR}/bin)
