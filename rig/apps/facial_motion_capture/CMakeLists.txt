cmake_minimum_required(VERSION 2.8)

project(FaceMoCap)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/client/include)

add_executable(FaceMoCap FacialMotionCapture.cpp)

target_link_libraries(FaceMoCap ${OpenCV_LIBS} dlib::dlib AvatarClient)

SET(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")

